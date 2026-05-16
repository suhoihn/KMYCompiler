#pragma once
#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <iostream>
#include <unordered_map>
#include "AstBaseForward.hpp"
#include "operators.hpp"
#include "visitor.hpp"

enum class ExprKind {
    Literal,
    ArrayLiteral,
    RecordLiteral,
    Variable,
    UnaryExpr,
    BinaryExpr,
    Index,
    Call,
    Get,
    Assignment,
    FunctionExpr,
    ThisExpr,
    NewExpr
};


struct BaseExpr : ASTNode {
    // This allows the memory management of recursive types since destructor is called
    // when derived objects are destroyed.
    ExprKind kind;
    Type* type = nullptr;
    
    BaseExpr(ExprKind kind) : kind(kind) {}

    virtual void accept(Visitor& v) = 0;
};

// Double dispatch for visitors.
template <typename Derived, ExprKind k>
struct ExprHelper : BaseExpr {
    
    ExprHelper() : BaseExpr(k) {}

    virtual bool isLValue() const { return false; }
    void accept(Visitor& v) override {
        v.visit(static_cast<Derived&>(*this));
    }
};

// -----AST nodes for expressions

// Literals for int, double, bool, string, null
struct Literal : ExprHelper<Literal, ExprKind::Literal> {
    std::variant<int, double, bool, std::string, std::nullptr_t> value;

    Literal();
    Literal(int i);
    Literal(double d);
    Literal(bool b);
    Literal(const std::string& s);
    Literal(std::nullptr_t);
};

struct ArrayLiteral : ExprHelper<ArrayLiteral, ExprKind::ArrayLiteral> {
    std::vector<ExprPtr> elements;

    // Take by value since unique_ptr is not copiable.
    ArrayLiteral(std::vector<ExprPtr> elements);
};

struct RecordLiteral : ExprHelper<RecordLiteral, ExprKind::RecordLiteral> {
    std::unordered_map<std::string, ExprPtr> fields;

    RecordLiteral(std::unordered_map<std::string, ExprPtr> fields);
};

struct Variable : ExprHelper<Variable, ExprKind::Variable> { 
    std::string name;
    
    bool resolved = false;
    ResolvedVar resolution;

    SymbolPtr symbol = nullptr; // Semantic info. Initially empty.

    // Overriding this since variables are assignable.
    virtual bool isLValue() const { return true; }
    
    Variable(const std::string& name);
};

struct UnaryExpr : ExprHelper<UnaryExpr, ExprKind::UnaryExpr>{
    UnaryOp op;
    ExprPtr operand;   

    UnaryExpr(UnaryOp op, ExprPtr operand);
};

struct BinaryExpr : ExprHelper<BinaryExpr, ExprKind::BinaryExpr> {
    BinaryOp op;
    ExprPtr left;
    ExprPtr right;

    BinaryExpr(BinaryOp op, ExprPtr left, ExprPtr right);
};

// Format: obj '[' index ']'
struct Index : ExprHelper<Index, ExprKind::Index> {
    ExprPtr obj;
    ExprPtr index; 

    virtual bool isLValue() const { return true; }

    Index(ExprPtr obj, ExprPtr index);
};

// Format: func '(' args ')'
struct Call : ExprHelper<Call, ExprKind::Call> {
    ExprPtr func;
    std::vector<ExprPtr> args; 

    Call(ExprPtr func, std::vector<ExprPtr> args);
};

// Format: obj '.' name
struct Get : ExprHelper<Get, ExprKind::Get> {
    ExprPtr obj;
    std::string name; // TODO change all these string fields to TOKENS for debugging.

    virtual bool isLValue() const { return true; }

    Get(ExprPtr obj, const std::string& name);
};

// Surprisingly, assignment can be an expression.
struct Assignment : ExprHelper<Assignment, ExprKind::Assignment> {
    AssignmentOp op; // e.g., +=, -=, etc.
    ExprPtr left;
    ExprPtr right;

    Assignment(AssignmentOp op, ExprPtr left, ExprPtr right);
};

// Parameter struct is in ASTBaseForward.hpp

// Kinda like lambda fun
struct FunctionExpr : ExprHelper<FunctionExpr, ExprKind::FunctionExpr> {
    std::vector<Parameter> params;
    StmtPtr body;
    TypeNodePtr returnType;

    Scope* scope = nullptr;
    std::vector<UpvalueInfo> upvalues;
    int frameSize = 0;

    FunctionExpr(const std::vector<Parameter>& params, StmtPtr body, TypeNodePtr returnType);
};

struct ThisExpr : ExprHelper<ThisExpr, ExprKind::ThisExpr> {};

struct NewExpr : ExprHelper<NewExpr, ExprKind::NewExpr> {
    std::string typeName;
    std::vector<ExprPtr> args;

    NewExpr(std::string typeName, std::vector<ExprPtr> args);
};


// -----AST nodes for statements
struct BaseStmt : ASTNode {
    virtual ~BaseStmt() = default;
    virtual void accept(Visitor& v) = 0;
};

template <typename Derived>
struct StmtHelper : BaseStmt {
    void accept(Visitor& v) override {
        v.visit(static_cast<Derived&>(*this));
    }
};

struct Print : StmtHelper<Print> { 
    ExprPtr expr; 

    Print(ExprPtr expr);
};

struct If : StmtHelper<If> {
    ExprPtr condition;
    StmtPtr thenbranch;
    StmtPtr elsebranch; // Optional else branch

    If(ExprPtr condition, StmtPtr thenbranch, StmtPtr elsebranch);
};

struct While : StmtHelper<While> {
    ExprPtr condition;
    StmtPtr body;

    While(ExprPtr condition, StmtPtr body);
};

struct Break : StmtHelper<Break> {};
struct Continue : StmtHelper<Continue> {};

struct Block : StmtHelper<Block> {
    std::vector<StmtPtr> statements;

    Scope* scope = nullptr;

    Block(std::vector<StmtPtr> statements);
};

struct Let : StmtHelper<Let> {
    TypeNodePtr type;
    std::string name;
    ExprPtr expr;
    bool isMutable;
    SymbolPtr symbol = nullptr;

    Let(TypeNodePtr type, const std::string& name, ExprPtr expr, bool isMutable);
};

struct Return : StmtHelper<Return> {
    ExprPtr expr; // Optional.
    
    Return(ExprPtr expr);
};

struct Member {
    virtual ~Member() = default;
};

struct FieldMember : Member {
    std::string name;
    ExprPtr initialiser; // Optional
    bool isMutable;

    FieldMember(std::string name, ExprPtr initialiser, bool isMutable);
};

struct MethodMember : Member {
    std::string name;
    std::shared_ptr<FunctionExpr> methodExpr;

    MethodMember(std::string name, std::shared_ptr<FunctionExpr> methodExpr);    
};

struct Class : StmtHelper<Class> {
    std::string name;
    std::vector<FieldMember> fieldMembers; // fields 
    std::vector<MethodMember> methodMembers; // methods (which are actually let stmts)

    SymbolPtr symbol = nullptr;
    Scope* scope = nullptr;

    Class (
        std::string name,
        std::vector<FieldMember> fieldMembers, 
        std::vector<MethodMember> methodMembers
    );
};

struct ExprStmt : StmtHelper<ExprStmt> {
    ExprPtr expr;
    ExprStmt(ExprPtr expr);
};



// std::variant is a type-safe union that can hold one of several types.
// using Stmt = std::variant<Print, If, While, Block, ExprStmt>;
