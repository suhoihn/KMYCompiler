#include "Resolver.hpp"

#include "../Core/errorhandler.hpp"

Resolver::Resolver(
    std::vector<StmtPtr> program,
    ScopePtr globalScope
)
    : program(std::move(program)),
      globalScope(std::move(globalScope)),
      currScope(globalScope.get())
{}

void Resolver::resolve() {
    for (auto& stmt : program) {
        stmt->accept(*this);
    }
}


SymbolPtr Resolver::resolveSymbol(const std::string& name) {
    Scope* scope = currScope;

    while (scope) {
        auto it = scope->symbols.find(name);

        if (it != scope->symbols.end()) {
            return it->second;
        }

        scope = scope->parent.get();
    }

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

    if (!sym) {
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