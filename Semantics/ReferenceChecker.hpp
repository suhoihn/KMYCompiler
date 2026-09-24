#pragma once

#include "../Core/Ast.hpp"
#include "../Utils/DefaultVisitor.hpp"

struct Module;

// Whether a binding currently owns a value that may be consumed or read.
// `MaybeUnavailable` is produced when control-flow paths disagree, such as
// when one branch moves a value and another branch leaves it available.
enum class BindingAvailability {
    Uninitialized,
    Available,
    Moved,
    MaybeUnavailable
};

// The type decides what a consuming context does to its source binding.
enum class OwnershipKind {
    Copy,
    Unique,
    Shared
};

// Semantic binding/reference-safety pass.
//
// This pass intentionally starts as a traversal-only skeleton because KMY does
// not have a source-level T& type yet.  It runs after Resolver, so every
// expression already has its Type and every variable use has its VarSymbol.
// Mutability is enforced here now; future reference rules also belong here
// rather than in IRBuilder/code generation:
//   - &expr requires stable lvalue storage;
//   - a returned reference cannot originate from a dead local;
//   - references cannot escape through fields/globals/closures unless allowed;
//   - moved owners cannot be used again.

using AvailabilityMap = std::unordered_map<const VarSymbol*, BindingAvailability>;
class ReferenceChecker : public DefaultVisitor {
public:
    explicit ReferenceChecker(Module& module);

    void check();

private:
    Module& module;
    FunctionExpr* currentFunction = nullptr;

    AvailabilityMap currentMap;
    
    OwnershipKind ownershipKind(const Type* type) const;
    void requireAvailable(const VarSymbol* symbol, const std::string& name) const;
    void consumeValue(const ExprPtr& expression);
    void requireMutableLValue(const ExprPtr& expression, const char* operation);

    // Only nodes that enforce mutability now or will create/transfer/merge
    // ownership state are overridden. Everything else uses DefaultVisitor.
    void visit(Variable& e) override;
    void visit(UnaryExpr& e) override;
    void visit(Assignment& e) override;
    void visit(FunctionExpr& e) override;
    void visit(Let& s) override;
    void visit(Return& s) override;
};
