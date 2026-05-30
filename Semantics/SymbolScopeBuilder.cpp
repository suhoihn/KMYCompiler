#include "SymbolScopeBuilder.hpp"

#include <vector>
#include "../Core/errorhandler.hpp"
#include "../Core/Ast.hpp"
#include <unordered_set>

SymbolScopeBuilder::SymbolScopeBuilder(
    FunctionExprPtr program
) : program(program)
{
    // Global scope made
    globalScope = new Scope();
    currScope = globalScope;
}

// Exports the scope tree.
Scope* SymbolScopeBuilder::analyse() {
    program->accept(*this);
    return globalScope;
}

void SymbolScopeBuilder::enterScope() {
    auto child = new Scope();
    child->parent = currScope;
    child->depth = currScope->depth + 1;
    currScope = child;
}

void SymbolScopeBuilder::exitScope() {
    currScope = currScope->parent;
}

SymbolPtr SymbolScopeBuilder::declare(const std::string& name, bool isMutable) {
    // 1. Check current scope only (NOT parents)
    if (currScope->symbols.find(name) != currScope->symbols.end()) {
        return nullptr; // redeclaration in same scope.
    }

    // 2. Create symbol
    SymbolPtr sym = std::make_shared<Symbol>(
        name,
        isMutable
    );

    // 3. Store in scope
    currScope->symbols[name] = sym;

    return sym;
}
    
TypeSymbol* SymbolScopeBuilder::declareType(const std::string& name, bool isMutable) {
    // New types can only be declared via aggregate (class or record) or typealias.
    
    // 1. Check current scope only (NOT parents)
    if (currScope->types.find(name) != currScope->types.end()) {
        throw KMYCompileError("Redeclaration of the same type \"" + name + "\""); // redeclaration in same scope.
    }

    TypeSymbol* sym = new TypeSymbol {name, nullptr};

    // 2. Store in scope.
    currScope->types[name] = sym;

    return sym;
}

void SymbolScopeBuilder::visit(Literal&) {}
void SymbolScopeBuilder::visit(Variable&) {}
void SymbolScopeBuilder::visit(ArrayLiteral& e) {
    for (auto& elem : e.elements)
        elem->accept(*this);
}
void SymbolScopeBuilder::visit(RecordLiteral& e) {
    std::unordered_set<std::string> seen;

    for (auto& [fieldName, value] : e.fields) {
        if (!seen.insert(fieldName).second) {
            throw KMYCompileError(
                "Duplicate field name \"" + fieldName + "\" in record literal."
            );
        }

        value->accept(*this);
    }
}
void SymbolScopeBuilder::visit(BinaryExpr& e) {
    e.left->accept(*this);
    e.right->accept(*this);
}
void SymbolScopeBuilder::visit(UnaryExpr& e) {
    e.operand->accept(*this);
}
void SymbolScopeBuilder::visit(Assignment& e) {
    e.left->accept(*this);
    e.right->accept(*this);
}
void SymbolScopeBuilder::visit(Index& e) {
    e.obj->accept(*this);
    e.index->accept(*this);
}
void SymbolScopeBuilder::visit(Call& e) {
    e.func->accept(*this);

    for (auto& arg : e.args)
        arg->accept(*this);
}
void SymbolScopeBuilder::visit(Get& e) {
    e.obj->accept(*this);
}
void SymbolScopeBuilder::visit(ThisExpr&) {}
void SymbolScopeBuilder::visit(NewExpr& e) {
    for (auto& arg : e.args)
        arg->accept(*this);
}

void SymbolScopeBuilder::visit(FunctionExpr& e) {
    bool isRoot = (&e == program.get());

    if (!isRoot) {
        enterScope();
    }
    e.scope = currScope;

    for (auto& param : e.params) {
        SymbolPtr sym = declare(param.name, param.isMutable);

        if (!sym) {
            throw KMYCompileError(
                "Redeclaration of parameter \"" + param.name + "\""
            );
        }
        param.symbol = sym;
    }

    e.body->accept(*this);

    if (!isRoot) {
        exitScope();
    }
}

// ======================================================
// Statements
// ======================================================

void SymbolScopeBuilder::visit(Block& s) {
    bool isRootBody = (&s == program->body.get());

    if (!isRootBody)
        enterScope();

    s.scope = currScope;

    for (auto& stmt : s.statements)
        stmt->accept(*this);

    if (!isRootBody)
        exitScope();
}

void SymbolScopeBuilder::visit(Print& s) {
    s.expr->accept(*this);
}

void SymbolScopeBuilder::visit(If& s) {
    s.condition->accept(*this);

    s.thenbranch->accept(*this);

    if (s.elsebranch)
        s.elsebranch->accept(*this);
}

void SymbolScopeBuilder::visit(While& s) {
    s.condition->accept(*this);
    s.body->accept(*this);
}

void SymbolScopeBuilder::visit(Break&) {}
void SymbolScopeBuilder::visit(Continue&) {}
void SymbolScopeBuilder::visit(Return& s) {
    if (s.expr)
        s.expr->accept(*this);
}

void SymbolScopeBuilder::visit(Let& s) {
    SymbolPtr sym = declare(s.name, s.isMutable);

    if (!sym) {
        throw KMYCompileError(
            "Redeclaration of variable \"" + s.name + "\""
        );
    }

    s.symbol = sym;

    if (s.expr)
        s.expr->accept(*this);
}

void SymbolScopeBuilder::visit(Aggregate& s) {
    // Declare class in outer scope (maybe not.)
    // SymbolPtr sym = declare(s.name, false);

    // class is immutable type?
    s.typeSymbol = declareType(s.name, false);

    /*
    if (!sym) {
        throw KMYCompileError(
            "Redeclaration of " + 
            std::string(s.kind == AggregateKind::RECORD ? "record" : "class") +
            " \"" + s.name + "\""
        );
    }

    s.symbol = sym;
    */

    // Class scope
    enterScope();

    s.scope = currScope;

    // Fields
    for (auto& member : s.fieldMembers) {
        SymbolPtr fieldSym = declare(member.name, member.isMutable);

        if (!fieldSym) {
            throw KMYCompileError(
                "Redeclaration of field \"" + member.name + "\""
            );
        }

        member.symbol = fieldSym;

        if (member.initialiser)
            member.initialiser->accept(*this);
    }

    // Methods
    for (auto& member : s.methodMembers) {
        // Methods are not mutable.
        SymbolPtr methodSym = declare(member.name, false);

        if (!methodSym) {
            throw KMYCompileError(
                "Redeclaration of method \"" + member.name + "\""
            );
        }

        member.symbol = methodSym;
        member.methodExpr->accept(*this);
    }

    s.scope = currScope;

    exitScope();
}

void SymbolScopeBuilder::visit(TypeAlias& s) {
    // STUB
    s.typeSymbol = declareType(s.name, false);
}

void SymbolScopeBuilder::visit(ExprStmt& s) {
    s.expr->accept(*this);
}