#include "Resolver.hpp"

#include "../Core/errorhandler.hpp"

Resolver::Resolver(
    FunctionExprPtr program,
    Scope* _globalScope
)
    : program(program),
      globalScope(_globalScope),
      currScope(globalScope)
{
    //std::cout << currScope << std::endl;
}

void Resolver::resolve() {
    program->accept(*this);
}


SymbolPtr Resolver::resolveSymbol(const std::string& name) {
    Scope* scope = currScope;
    std::cout << "[DEBUG] Resolving symbol: " << name << std::endl;
    std::cout << scope << std::endl;
    while (scope) {
        std::cout << scope << std::endl;
        auto it = scope->symbols.find(name);

        if (it != scope->symbols.end()) {
            return it->second;
        }

        scope = scope->parent;
    }
    std::cout << "[DEBUG] Symbol not found: " << name << std::endl;
    return nullptr; // Undefined variable.
}


void Resolver::visit(Literal&) {}

void Resolver::visit(ArrayLiteral& e) {
    for (auto& elem : e.elements)
        elem->accept(*this);
}

void Resolver::visit(RecordLiteral& e) {
    for (auto& [_, value] : e.fields)
        value->accept(*this);
}

void Resolver::visit(Variable& e) {
    SymbolPtr sym = resolveSymbol(e.name);
    std::cout << "[DEBUG] Resolved symbol: " << sym << std::endl;

    if (!sym) {
        std::cout << "Throw?" << e.name <<  std::endl;
        throw KMYCompileError(
            "Undefined variable \"" + e.name + "\""
        );
    }

    e.symbol = sym;
}

void Resolver::visit(BinaryExpr& e) {
    e.left->accept(*this);
    e.right->accept(*this);
}

void Resolver::visit(UnaryExpr& e) {
    e.operand->accept(*this);
}

void Resolver::visit(Assignment& e) {
    e.left->accept(*this);
    e.right->accept(*this);
}

void Resolver::visit(Index& e) {
    e.obj->accept(*this);
    e.index->accept(*this);
}

void Resolver::visit(Call& e) {
    e.func->accept(*this);

    for (auto& arg : e.args)
        arg->accept(*this);
}

void Resolver::visit(Get& e) {
    e.obj->accept(*this);
}

void Resolver::visit(FunctionExpr& e) {
    Scope* old = currScope;

    currScope = e.scope; // scope created in Pass 1
    std::cout << "func scope: " << currScope << std::endl;
    for (const auto& [name, sym] : currScope->symbols) {
        std::cout << "  " << name << " (mutable: " << sym->isMutable << ")\n";
    }

    e.body->accept(*this);

    currScope = old;
}

void Resolver::visit(ThisExpr&) {}

void Resolver::visit(NewExpr& e) {
    for (auto& arg : e.args)
        arg->accept(*this);
}


// Statements
void Resolver::visit(Print& s) {
    s.expr->accept(*this);
}

void Resolver::visit(If& s) {
    s.condition->accept(*this);

    s.thenbranch->accept(*this);

    if (s.elsebranch)
        s.elsebranch->accept(*this);
}

void Resolver::visit(While& s) {
    loopDepth++;

    s.condition->accept(*this);
    s.body->accept(*this);

    loopDepth--;
}

void Resolver::visit(Block& s) {
    Scope* old = currScope;

    currScope = s.scope; // assigned in Pass 1
    std::cout << "block scope: " << currScope << std::endl;
    for (const auto& [name, sym] : currScope->symbols) {
        std::cout << "  " << name << " (mutable: " << sym->isMutable << ")\n";
    }

    for (auto& stmt : s.statements)
        stmt->accept(*this);

    currScope = old;
}

void Resolver::visit(Break&) {
    if (loopDepth <= 0) {
        // Not inside a loop.
        throw KMYCompileError("Invalid break position.");
    }
}
void Resolver::visit(Continue&) {
    if (loopDepth <= 0) {
        // Not inside a loop.
        throw KMYCompileError("Invalid continue position.");
    }
}

void Resolver::visit(Let& s) {
    if (s.expr)
        s.expr->accept(*this);
}

void Resolver::visit(Return& s) {
    if (currScope->depth <= 0) {
        // Not in a function
        KMYCompileError("Invalid return statement. Not in a function.");
    }

    if (s.expr)
        s.expr->accept(*this);
}

void Resolver::visit(Class& s) {
    Scope* old = currScope;

    currScope = s.scope;

    for (auto& member : s.fieldMembers) {
        if (member.initialiser)
            member.initialiser->accept(*this);

    }

    for (auto& member : s.methodMembers)
        member.methodExpr->accept(*this);

    currScope = old;
}

void Resolver::visit(ExprStmt& s) {
    s.expr->accept(*this);
}