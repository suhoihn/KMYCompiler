#include "Ast.hpp"

#include <string>
#include <unordered_map>
#include "operators.hpp"

// -----Expressions
Literal::Literal() : value(nullptr) {}
Literal::Literal(int i) : value(i) {}
Literal::Literal(double d) : value(d) {}
Literal::Literal(bool b) : value(b) {}
Literal::Literal(const std::string& s) : value(s) {}
Literal::Literal(std::nullptr_t) : value(nullptr) {}
ArrayLiteral::ArrayLiteral(std::vector<ExprPtr> elements) : elements(move(elements)) {}
RecordLiteral::RecordLiteral(std::unordered_map<std::string, ExprPtr> fields) : fields(move(fields)) {}
Variable::Variable(const std::string& name) : name(name) {}
UnaryExpr::UnaryExpr(UnaryOp op, ExprPtr operand) : op(op), operand(move(operand)) {}
BinaryExpr::BinaryExpr(BinaryOp op, ExprPtr left, ExprPtr right) : op(op), left(move(left)), right(move(right)) {}
Assignment::Assignment(AssignmentOp op, ExprPtr left, ExprPtr right) : op(op), left(move(left)), right(move(right)) {}
Index::Index(ExprPtr obj, ExprPtr index) : obj(move(obj)), index(move(index)) {}
Call::Call(ExprPtr func, std::vector<ExprPtr> args) : func(move(func)), args(move(args)) {}
Get::Get(ExprPtr obj, const std::string& name) : obj(move(obj)), name(name) {}
FunctionExpr::FunctionExpr(const std::vector<Parameter>& params, StmtPtr body, TypeNodePtr returnType) 
    : params(move(params)), body(move(body)), returnType(std::move(returnType)) {}
// Nothing for ThisLiteral.
NewExpr::NewExpr(std::string typeName, std::vector<ExprPtr> args) : typeName(move(typeName)), args(move(args)) {}

// -----Statements
Print::Print(ExprPtr expr) : expr(move(expr)) {}
If::If(ExprPtr condition, StmtPtr thenbranch, StmtPtr elsebranch=nullptr) : condition(move(condition)), thenbranch(move(thenbranch)), elsebranch(move(elsebranch)) {}
While::While(ExprPtr condition, StmtPtr body) : condition(move(condition)), body(move(body)) {}
// Nothing for Break
// Nothing for Continue
Block::Block(std::vector<StmtPtr> statements) : statements(move(statements)) {}
Let::Let(TypeNodePtr type, const std::string& name, ExprPtr expr, bool isMutable) 
    : type(std::move(type)), name(name), expr(move(expr)), isMutable(isMutable) {}
Return::Return(ExprPtr expr) : expr(move(expr)) {}
FieldMember::FieldMember(std::string name, ExprPtr initialiser, bool isMutable) 
    : name(move(name)), initialiser(move(initialiser)), isMutable(isMutable) {}

MethodMember::MethodMember(std::string name, std::shared_ptr<FunctionExpr> methodExpr) 
    : name(move(name)), methodExpr(move(methodExpr)) {}

Class::Class (
    std::string name,
    std::vector<FieldMember> fieldMembers, 
    std::vector<MethodMember> methodMembers
) : name(move(name)), fieldMembers(move(fieldMembers)), methodMembers(move(methodMembers)) {}

ExprStmt::ExprStmt(ExprPtr expr) : expr(move(expr)) {}
