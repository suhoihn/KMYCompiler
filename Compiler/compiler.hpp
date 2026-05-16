#pragma once

#include <vector>
#include "../Core/Ast.hpp"
#include "../Core/value.hpp"
#include "../Core/visitor.hpp"
#include "../BytecodeVM/vm.hpp"
#include "../Semantics/Resolver.hpp"






struct FunctionContext {
    FunctionContext* parent;

    Chunk chunk;
    
    std::vector<Local> locals;
    std::unordered_map<std::string, int> localMap;
    
    std::unordered_map<ConstValue, int> constantMap; // Constants are accessed via chunk.constants

    int scopeDepth = 0;
    int nextSlot = 0;

    std::unordered_map<std::string, int> upvalueMap;
    std::vector<UpvalueInfo> upvalues;
};

class Compiler : public Visitor {
public:
    Compiler(const std::vector<StmtPtr>& program);
    Chunk compile(void);
private:
    // const Resolver& resolver;
    const std::vector<StmtPtr> program; // ASTs (read-only)

    ResolvedVar Compiler::resolveVariable(const std::string& name);
   
    void emit(Opcode op, int operand);
    
    // Tracking current function context.
    FunctionContext* currCtx;

    // Function Prototypes (immutable function codes)
    int allocateFuncProto(const FunctionProto& fnProto);
    std::vector<FunctionProto> funcProtos;
    int funcProtoCnt = 0;

    // Globals
    std::unordered_map<std::string, int> globals;

    // For locals    
    int allocateLocal(SymbolPtr sym);

    // For constants
    int addConstant(const ConstValue& v);

    // For branches
    int emitJump(Opcode op);
    void patchJump(int pos);

    // Expressions
    void visit(Literal& e) override;
    void visit(ArrayLiteral& e) override;
    void visit(RecordLiteral& e) override;
    void visit(Variable& e) override;
    void visit(BinaryExpr& e) override;
    void visit(UnaryExpr& e) override;
    void visit(Assignment& e) override;
    void visit(Index& e) override;
    void visit(Call& e) override;
    void visit(Get& e) override;
    void visit(FunctionExpr& e) override;
    void visit(ThisExpr& e) override;
    void visit(NewExpr& e) override;

    // Statements
    void visit(Print& s) override;
    void visit(If& s) override;
    void visit(While& s) override;
    void visit(Block& s) override;
    void visit(Break& s) override;
    void visit(Continue& s) override;
    void visit(Let& s) override;
    void visit(Return& s) override;
    void visit(Class& s) override;
    void visit(ExprStmt& s) override;
};