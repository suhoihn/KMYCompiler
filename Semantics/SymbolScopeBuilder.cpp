#include "SymbolScopeBuilder.hpp"

#include <vector>
#include <unordered_set>
#include "../Core/errorhandler.hpp"
#include "../Core/Ast.hpp"
#include "../Utils/NativeFunctionImpl.hpp"
#include "../Utils/utils.hpp"
#include <assert.h>

SymbolScopeBuilder::SymbolScopeBuilder(
    FunctionExprPtr program
) : program(program)
{
    // Global scope made
    globalScope = new Scope();
    currScope = globalScope;

    // Build native functions HERE(?)
    return;
    for (auto& [name, info] : nativeFnTypes) {
        VarSymbol* sym = declareVar(name, false);

        sym->type = info.type;
        sym->nativeFnPtr = info.fn;
        sym->globalSlot = info.globalSlot; // Hacky?
    }
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

VarSymbol* SymbolScopeBuilder::declareVar(const std::string& name, bool isMutable) {
    // 1. Check current scope only (NOT parents)
    if (currScope->values.find(name) != currScope->values.end()) {
        // Redeclaration in same scope.
        return nullptr; 
    }

    // 2. Create symbol
    VarSymbol* sym = new VarSymbol(name, isMutable);
    sym->definingScope = currScope;

    // 3. Store in scope
    currScope->values[name] = sym;

    return sym;
}
    
TypeSymbol* SymbolScopeBuilder::declareType(const std::string& name, bool isMutable) {
    // NOTE: New types can only be declared via aggregate (class or record) or typealias.
    // TODO: Implement const types (need usage for isMutable.)
    printLog(LogLevel::DEBUG, "Declaring type symbol " + name + "\n");
    // 1. Check current scope only (NOT parents)
    if (currScope->types.find(name) != currScope->types.end()) {
        // Redeclaration in same scope.
        return nullptr;
        //throw KMYCompileError("Redeclaration of the same type \"" + name + "\""); 
    }

    TypeSymbol* sym = new TypeSymbol(name, isMutable);

    // 2. Store in scope.
    currScope->types[name] = sym;

    return sym;
}

// Check duplicate fields.
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

// Function expr (or decl) has a scope, and each parameter has a var symbol.
void SymbolScopeBuilder::visit(FunctionExpr& e) {
    bool isRoot = (&e == program.get());

    if (!isRoot) {
        enterScope();
    }
    e.scope = currScope;

    for (auto& param : e.params) {
        VarSymbol* sym = declareVar(param.name, param.isMutable);

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

// Block has a scope.
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


// Let stmt has a var symbol.
void SymbolScopeBuilder::visit(Let& s) {
    VarSymbol* sym = declareVar(s.name, s.isMutable);
    
    if (!sym) {
        throw KMYCompileError(
            "Redeclaration of variable \"" + s.name + "\""
        );
    }
    
    // Function decls always available in the current scope.
    if (s.isFunctionDecl) {
        sym->available = true;
    }

    s.symbol = sym;

    if (s.expr)
        s.expr->accept(*this);
}

// Aggregate has a type symbol AND a scope.
// Each field member also has a var symbol.
void SymbolScopeBuilder::visit(Aggregate& s) {
    // Declare class in outer scope (maybe not.)
    // VarSymbol* sym = declareVar(s.name, false);

    // class is immutable type?
    auto sym = declareType(s.name, false);

    if (!sym) {
        throw KMYCompileError(
            "Redeclaration of type \"" + 
            // std::string(s.kind == AggregateKind::RECORD ? "record" : "class") +
            s.name + "\""
        );
    }

    s.typeSymbol = sym;
    

    // Class scope
    enterScope();

    s.scope = currScope;

    // Fields
    for (auto& member : s.fieldMembers) {
        VarSymbol* fieldSym = declareVar(member.name, member.isMutable);
        
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
        VarSymbol* methodSym = declareVar(member.name, false);
        member.symbol = methodSym;

        if (!methodSym) {
            throw KMYCompileError(
                "Redeclaration of method \"" + member.name + "\""
            );
        }

        // Create "this" parameter (should be unique for each method)
        Parameter thisParam(nullptr, "$implicit_this_param", false, false, false);
        thisParam.implicitThis = true;

        auto& paramVec = member.methodExpr->params;
        assert(
            paramVec.empty() ||
            paramVec.front().name != "$implicit_this_param"
        );
        paramVec.insert(paramVec.begin(), thisParam);

        member.methodExpr->accept(*this);
    }

    // Constructors
    int cnt = 0;
    for (auto& member : s.constructorMembers) {
        // Constructors are not mutable.
        VarSymbol* constrSym = declareVar("implicit_init" + std::to_string(cnt++), false);

        member.symbol = constrSym;

        // Create "this" parameter (should be unique for each method)
        Parameter thisParam(nullptr, "$implicit_this_param", false, false, false);
        thisParam.implicitThis = true;

        auto& paramVec = member.initFuncExpr->params;
        assert(
            paramVec.empty() ||
            paramVec.front().name != "$implicit_this_param"
        );
        paramVec.insert(paramVec.begin(), thisParam);

        member.initFuncExpr->accept(*this);
    }

    // Enums (TODO: support METHODS)
    for (auto& member : s.enumMembers) {
        // TODO: Combine symbol instead of VarSymbol?
        // member.symbol is unused.
        member.customEnum->accept(*this);
    }

    // Field init func
    {
        // Create "this" parameter (should be unique for each method)
        Parameter thisParam(nullptr, "$implicit_this_param", false, false, false);
        thisParam.implicitThis = true;
        
        auto& paramVec = s.fieldInitFunc->params;
        s.fieldInitFunc->params.insert(paramVec.begin(), thisParam);
        s.fieldInitFunc->accept(*this);
    }

    s.scope = currScope;

    exitScope();
}

// Typealias has a type symbol.
void SymbolScopeBuilder::visit(TypeAlias& s) {
    // Simply register a type symbol. (Its Type* is nullptr initially)
    s.typeSymbol = declareType(s.name, false);
    s.typeSymbol->typeNode = s.aliasingType;
}

// Enum has a type symbol. (Its member has NO symbols!)
void SymbolScopeBuilder::visit(Enum& s) {
    // Similar to classes. Enum builds a type
    s.typeSymbol = declareType(s.name, false);
    
    std::unordered_set<std::string> seen;
    for (auto& str : s.variants) {
        if (!seen.insert(str).second) {
            throw KMYCompileError(
                "Duplicate enum variant \"" + str + "\"."
            );
        }
    }
}
