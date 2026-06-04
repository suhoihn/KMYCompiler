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


struct BaseExpr : public ASTNode {
    // This allows the memory management of recursive types since destructor is called
    // when derived objects are destroyed.
    ExprKind kind;
    Type* type = nullptr;
    
    BaseExpr(ExprKind kind) : kind(kind) {}
    
    virtual bool isLValue() const { return false; }
    virtual void accept(Visitor& v) = 0;
    // virtual ExprPtr clone() const = 0;
};

// Double dispatch for visitors.
template <typename Derived, ExprKind k>
struct ExprHelper : public BaseExpr {
    
    ExprHelper() : BaseExpr(k) {}

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

    // ExprPtr clone() const override;
};

struct ArrayLiteral : ExprHelper<ArrayLiteral, ExprKind::ArrayLiteral> {
    std::vector<ExprPtr> elements;

    ArrayLiteral(std::vector<ExprPtr> elements);

    // ExprPtr clone() const override;
};

struct RecordLiteral : ExprHelper<RecordLiteral, ExprKind::RecordLiteral> {
    std::vector<std::pair<std::string, ExprPtr>> fields;
    std::unordered_map<std::string, int> layout; // field name to index mapping for codegen

    RecordLiteral(std::vector<std::pair<std::string, ExprPtr>> fields);
    // ExprPtr clone() const override;
};

struct Variable : ExprHelper<Variable, ExprKind::Variable> { 
    std::string name;
    
    bool resolved = false;
    ResolvedVar resolution;

    SymbolPtr symbol = nullptr; // Semantic info. Initially empty.

    // Overriding this since variables are assignable.
    virtual bool isLValue() const { return true; }
    
    Variable(const std::string& name);

    // ExprPtr clone() const override;
};

struct UnaryExpr : ExprHelper<UnaryExpr, ExprKind::UnaryExpr>{
    UnaryOp op;
    ExprPtr operand;   

    UnaryExpr(UnaryOp op, ExprPtr operand);

    // ExprPtr clone() const override;
};

struct BinaryExpr : ExprHelper<BinaryExpr, ExprKind::BinaryExpr> {
    BinaryOp op;
    ExprPtr left;
    ExprPtr right;

    BinaryExpr(BinaryOp op, ExprPtr left, ExprPtr right);

    // ExprPtr clone() const override;
};

// Format: obj '[' index ']'
struct Index : ExprHelper<Index, ExprKind::Index> {
    ExprPtr obj;
    ExprPtr index; 

    virtual bool isLValue() const { return true; }

    Index(ExprPtr obj, ExprPtr index);

    // ExprPtr clone() const override;
};

// Format: func '(' args ')'
struct Call : ExprHelper<Call, ExprKind::Call> {
    ExprPtr func;
    std::vector<ExprPtr> args; 

    Call(ExprPtr func, std::vector<ExprPtr> args);

    // ExprPtr clone() const override;
};

// Format: obj '.' name
struct Get : ExprHelper<Get, ExprKind::Get> {
    ExprPtr obj;
    std::string name; // TODO change all these string fields to TOKENS for debugging.

    int fieldIdx; // For codegen, set by Resolver. Only for fields.
    int methodIdx; // For codegen and lowering, set by Resolver. Only for methods.
    
    // For lowering. Check if its a form of obj.f (obj is aggregate, f is method)
    bool resolvedMethod = false;

    virtual bool isLValue() const { return true; }

    Get(ExprPtr obj, const std::string& name);

    // ExprPtr clone() const override;
};

// Surprisingly, assignment can be an expression.
struct Assignment : ExprHelper<Assignment, ExprKind::Assignment> {
    AssignmentOp op; // e.g., +=, -=, etc.
    ExprPtr left;
    ExprPtr right;

    Assignment(AssignmentOp op, ExprPtr left, ExprPtr right);

    // ExprPtr clone() const override;
};

// Parameter struct is in ASTBaseForward.hpp

// Kinda like lambda fun
struct FunctionExpr : ExprHelper<FunctionExpr, ExprKind::FunctionExpr> {
    std::vector<Parameter> params;
    StmtPtr body;
    TypeNodePtr annotatedReturnType;

    SymbolPtr symbol = nullptr;
    Scope* scope = nullptr;
    std::vector<UpvalueInfo> upvalues;
    int frameSize = 0;
    int fnProtoIdx = INVALID_SLOT;

    FunctionExpr(const std::vector<Parameter>& params, StmtPtr body, TypeNodePtr annotatedReturnType);

    // ExprPtr clone() const override;
};

using FunctionExprPtr = std::shared_ptr<FunctionExpr>;

struct ThisExpr : ExprHelper<ThisExpr, ExprKind::ThisExpr> {
    // ExprPtr clone() const override;
};

struct NewExpr : ExprHelper<NewExpr, ExprKind::NewExpr> {
    std::string typeName;
    std::vector<ExprPtr> args;

    NewExpr(std::string typeName, std::vector<ExprPtr> args);

    // ExprPtr clone() const override;
};

// -----AST nodes for statements
struct BaseStmt : public ASTNode {
    virtual ~BaseStmt() = default;
    virtual void accept(Visitor& v) = 0;

    // virtual StmtPtr clone() const = 0;
};

template <typename Derived>
struct StmtHelper : public BaseStmt {
    void accept(Visitor& v) override {
        v.visit(static_cast<Derived&>(*this));
    }
};

struct Print : StmtHelper<Print> { 
    ExprPtr expr; 

    Print(ExprPtr expr);

    // StmtPtr clone() const override;
};

struct If : StmtHelper<If> {
    ExprPtr condition;
    StmtPtr thenbranch;
    StmtPtr elsebranch; // Optional else branch

    If(ExprPtr condition, StmtPtr thenbranch, StmtPtr elsebranch);

    // StmtPtr clone() const override;
};

struct While : StmtHelper<While> {
    ExprPtr condition;
    StmtPtr body;

    While(ExprPtr condition, StmtPtr body);

    // StmtPtr clone() const override;
};

struct Break : StmtHelper<Break> {
    // StmtPtr clone() const override;
};
struct Continue : StmtHelper<Continue> {
    // StmtPtr clone() const override;
};

struct Block : StmtHelper<Block> {
    std::vector<StmtPtr> statements;

    Scope* scope = nullptr;

    Block(std::vector<StmtPtr> statements);

    // StmtPtr clone() const override;
};

struct Let : StmtHelper<Let> {
    TypeNodePtr annotatedType;
    std::string name;
    ExprPtr expr;
    bool isMutable;

    SymbolPtr symbol = nullptr;

    Let(TypeNodePtr annotatedType, const std::string& name, ExprPtr expr, bool isMutable);
    // StmtPtr clone() const override;
};

struct Return : StmtHelper<Return> {
    ExprPtr expr; // Optional.
    
    Return(ExprPtr expr);
    // StmtPtr clone() const override; 
};

struct Member {
    virtual ~Member() = default;
    SymbolPtr symbol = nullptr;
};

struct FieldMember : Member {
    std::string name;
    ExprPtr initialiser; // Optional. but wont be used from now on (2026-06-62)
    bool isMutable;
    TypeNodePtr annotatedType;

    FieldMember(TypeNodePtr annotatedType, std::string name, ExprPtr initialiser, bool isMutable);
};

struct MethodMember : Member {
    std::string name;
    FunctionExprPtr methodExpr;

    MethodMember(std::string name, FunctionExprPtr methodExpr);    
};

struct ConstructorMember : Member {
    FunctionExprPtr initFuncExpr;

    ConstructorMember(FunctionExprPtr initFuncExpr);    
};


// Same syntax and semantics for records and classes.
enum class AggregateKind {
    RECORD,
    CLASS
};

struct Aggregate : StmtHelper<Aggregate> {
    AggregateKind kind;
    
    std::string name;
    std::vector<FieldMember> fieldMembers; // fields 
    std::vector<MethodMember> methodMembers; // methods
    std::vector<ConstructorMember> constructorMembers; // constructor functions
    
    FunctionExprPtr fieldInitFunc; // field initialisers.

    TypeSymbol* typeSymbol = nullptr;

    Scope* scope = nullptr;
    int fieldCount = 0;
    Aggregate (
        AggregateKind kind,
        std::string name,
        std::vector<FieldMember> fieldMembers, 
        std::vector<MethodMember> methodMembers,
        std::vector<ConstructorMember> constructorMembers,
        FunctionExprPtr fieldInitFunc
    );
};

struct TypeAlias : StmtHelper<TypeAlias> {
    std::string name;
    TypeSymbol* typeSymbol = nullptr;
    TypeNodePtr annotatedType;

    TypeAlias(std::string name, TypeNodePtr annotatedType);
};

struct ExprStmt : StmtHelper<ExprStmt> {
    ExprPtr expr;
    ExprStmt(ExprPtr expr);
    // StmtPtr clone() const override;
};

// -----AST nodes for declarations
struct BaseDecl : ASTNode {
    virtual ~BaseDecl() = default;
    virtual void accept(Visitor& v) = 0;
};

template <typename Derived>
struct DeclHelper : BaseDecl {
    void accept(Visitor& v) override {
        v.visit(static_cast<Derived&>(*this));
    }
};

// TO BE CONSIDERED: Move decls to separate wrappers. DeclStmt (Why? just for semantic categorisation.)

// std::variant is a type-safe union that can hold one of several types.

