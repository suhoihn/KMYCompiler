#pragma once

#include "../Core/Ast.hpp"
#include "../Utils/DefaultVisitor.hpp"

struct Module;

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
class ReferenceChecker : public DefaultVisitor {
public:
    explicit ReferenceChecker(Module& module);

    void check();

private:
    Module& module;
    FunctionExpr* currentFunction = nullptr;

    void requireMutableLValue(const ExprPtr& expression, const char* operation);

    // These overrides are the control points where reference provenance and
    // escape checks will be added once T& exists.  For now they only preserve
    // complete AST traversal and therefore do not change language behaviour.
    void visit(UnaryExpr& e) override;
    void visit(Assignment& e) override;
    void visit(FunctionExpr& e) override;
    void visit(Let& s) override;
    void visit(Return& s) override;
};
