#include "Resolver.hpp"

#include <iostream>
#include "../Core/errorhandler.hpp"
#include "../Utils/SymbolPrinter.hpp"
#include "../Utils/utils.hpp"
#include <assert.h>

Resolver::Resolver(
    FunctionExprPtr program,
    Scope* _globalScope
) : 
    program(program),
    globalScope(_globalScope),
    currScope(globalScope)
{}

void Resolver::resolve() {
    printLog(LogLevel::INFO, "Resolver pass started\n");
    program->accept(*this);
    printLog(LogLevel::INFO, "Resolver pass ended.\n");
}

VarSymbol* Resolver::declareVar(const std::string& name, bool isMutable) {
    // 1. Check current scope only (NOT parents)
    if (currScope->values.find(name) != currScope->values.end()) {
        // Redeclaration in same scope.
        return nullptr; 
    }

    // 2. Create symbol
    VarSymbol* sym = new VarSymbol(name, isMutable);

    // 3. Store in scope
    currScope->values[name] = sym;

    return sym;
}

VarSymbol* Resolver::resolveVarSymbol(const std::string& name) {
    Scope* scope = currScope;
    printLog(LogLevel::DEBUG, "Resolving var symbol: " + name + "\n");

    while (scope) {
        auto it = scope->values.find(name);

        if (it != scope->values.end()) {
            if (it->second->available) {
                return it->second;
            }
            printLog(LogLevel::DEBUG, "Var symbol used before let: " + name + "\n");
            return nullptr;
        }

        scope = scope->parent;
    }
    printLog(LogLevel::DEBUG, "Var symbol not found: " + name + "\n");
    return nullptr; // Undefined variable.
}

TypeSymbol* Resolver::resolveTypeSymbol(const std::string& name) {
    Scope* scope = currScope;
    printLog(LogLevel::DEBUG, "Resolving type symbol: " + name + "\n");

    while (scope) {
        auto it = scope->types.find(name);

        if (it != scope->types.end()) {
            return it->second;
        }

        scope = scope->parent;
    }

    printLog(LogLevel::DEBUG, "Type symbol not found: " + name + "\n");
    return nullptr; // Undefined type name.
}

static std::unordered_map<std::string, Type*> primitiveToType = {
    {"int", &Types::INT_TYPE},
    {"double", &Types::DOUBLE_TYPE},
    {"bool", &Types::BOOL_TYPE},
    {"string", &Types::STRING_TYPE},
    {"null", &Types::NULL_TYPE},
    {"void", &Types::VOID_TYPE},
    {"any", &Types::ANY_TYPE},
};

Type* Resolver::typeSigToType(const TypeNodePtr type) {
    printLog(LogLevel::DEBUG, "Converting type annotation to Type*: (TODO...)\n" );
    std::cout << type << "\n";
    
    if (!type) {
        throw KMYCompileError("Missing type annotation.");
    }
    switch (type->kind) {
        case TypeNodeKind::NAMED: {
            std::cout << "ur named\n";
            const auto& named = static_cast<NamedTypeNode&>(*type);

            std::cout << "con succ\n";
            auto it = primitiveToType.find(named.name);
            if (it != primitiveToType.end()) {
                std::cout << "found!\n";
                return it->second;
            }
            throw KMYCompileError("Serious error. primitive not handled?");
        }

        case TypeNodeKind::ARRAY: {
            std::cout << "ur array\n";
            const auto& arrayTypeNode = static_cast<ArrayTypeNode&>(*type);
            Type* elementType = typeSigToType(arrayTypeNode.elementType);
            return TypeInterner::getArrayType(elementType);
        }

        case TypeNodeKind::FUNCTION: {
            std::cout << "ur function\n";
            const auto& funcTypeNode = static_cast<FunctionTypeNode&>(*type);
            std::vector<Type*> paramTypes;
            for (const auto& param : funcTypeNode.params) {
                paramTypes.push_back(typeSigToType(param));
            }
            Type* returnType = typeSigToType(funcTypeNode.returnType);
            // IMPORTANT NOTE: No param info is preserved.
            return TypeInterner::getFunctionType(std::move(paramTypes), returnType);
        }

        case TypeNodeKind::RECORD: {
            std::cout << "ur record\n";
            const auto& recordTypeNode = static_cast<RecordTypeNode&>(*type);
            std::unordered_map<std::string, Type*> fieldTypes;

            for(auto& pair : recordTypeNode.paramTypePairs) {
                if (fieldTypes.count(pair.first)) {
                    throw KMYCompileError(
                        "Redeclaration of field name \"" + pair.first + "\" in type signature."
                    );
                }
                fieldTypes[pair.first] = typeSigToType(pair.second);
            }

            return TypeInterner::getStructualType(std::move(fieldTypes));
        }
        
        case TypeNodeKind::SCOPED: {
            std::cout << "ur scoped\n";
            const auto& scopedTypeNode = static_cast<ScopedTypeNode&>(*type);

            const std::string& firstPart = scopedTypeNode.scopeParts[0];
            std::cout << firstPart << " is the firstpart.\n";
            TypeSymbol* typeSym = resolveTypeSymbol(firstPart);
            if (!typeSym) {
                throw KMYCompileError("Unknown type symbol \"" + firstPart + "\" in scoped type");
            }

            Type* currType = typeSym->type;

            // start from SECOND element
            for (size_t i = 1; i < scopedTypeNode.scopeParts.size(); i++) {
                const std::string& currPart = scopedTypeNode.scopeParts[i];

                if (currType->kind != TypeKind::INSTANCE) {
                    throw KMYCompileError("Only aggregates support nested ::");
                }

                auto aggType = static_cast<InstanceType*>(currType);

                std::cout << "enummap contains:\n";
                for (auto& [f, _] : aggType->enumMap) {
                    std::cout << f << "\n";
                }

                auto it = aggType->enumMap.find(currPart);
                if (it == aggType->enumMap.end()) {
                    throw KMYCompileError(
                        "No type symbol \"" + currPart + "\" found in " + aggType->name
                    );
                }

                currType = it->second->type;
            }

            return currType;
        }
    }

    throw KMYCompileError("Severe: Unknown type node kind.");
}

void Resolver::visit(Literal& e) {
    e.type = std::visit([](auto&& value) -> Type* {
        using T = std::decay_t<decltype(value)>;

        if constexpr (std::is_same_v<T, int>)
            return &Types::INT_TYPE;
        else if constexpr (std::is_same_v<T, double>)
            return &Types::DOUBLE_TYPE;
        else if constexpr (std::is_same_v<T, bool>)
            return &Types::BOOL_TYPE;
        else if constexpr (std::is_same_v<T, std::string>)
            return &Types::STRING_TYPE;
        else if constexpr (std::is_same_v<T, std::nullptr_t>)
            return &Types::NULL_TYPE;
        else
            return nullptr;

    }, e.value);
}   

void Resolver::visit(ArrayLiteral& e) {
    printLog(LogLevel::DEBUG, "Visiting array literal node\n");

    Type* baseType = &Types::ANY_TYPE;
    bool elementExists = false;
    for (auto& elem : e.elements) {
        elem->accept(*this);

        if (elementExists) {
            if (baseType != elem->type && elem->type != &Types::ANY_TYPE) {
                throw KMYCompileError(
                    "Unmatched array element type (" 
                    + typeToString(elem->type) 
                    + ") from expected type (" 
                    + typeToString(baseType) + ")."
                );
            }
        } else {
            baseType = elem->type;
            elementExists = true;
        }
    }

    if (!elementExists) {
        if (!expectedType) {
            baseType = &Types::ANY_TYPE;
           // throw KMYCompileError("Cannot infer type of empty array literal without expected type. This must be a compiler issue lol.");
        } else {
            if (expectedType->kind != TypeKind::ARRAY) {
                throw KMYCompileError("Expected array type for array literal, got " + typeToString(expectedType));
            }
            auto arrayType = static_cast<ArrayType*>(expectedType);
            e.type = arrayType;
            return;
        }
    }

    e.type = TypeInterner::getArrayType(baseType);
}

void Resolver::visit(RecordLiteral& e) {
    std::unordered_map<std::string, Type*> fieldTypes;

    bool expectedTypeExists = false;
    StructualType* expected = nullptr;
    if (expectedType && expectedType->kind == TypeKind::STRUCTUAL) {
        expectedTypeExists = true;
        expected = static_cast<StructualType*>(expectedType);
    }

    Type* oldET = expectedType;

    for (auto& [fieldName, value] : e.fields) {
        if (expectedTypeExists) { expectedType = expected->fieldTypes[fieldName]; }
        
        value->accept(*this);
        fieldTypes[fieldName] = value->type;   
    }

    expectedType = oldET;

    for (auto& [name, slot] : e.layout) {
        std::cout << "name: " << name << "slot: " << slot << std::endl;
    }

    e.type = TypeInterner::getStructualType(std::move(fieldTypes));
}

void Resolver::visit(Variable& e) {
    VarSymbol* sym = resolveVarSymbol(e.name);

    if (!sym) {
        throw KMYCompileError(
            "Undefined variable \"" + e.name + "\""
        );
    }

    if (assigning && sym->type == &Types::UNINITIALISED) {
        throw KMYCompileError("Usage of uninitialised variable \"" + e.name + "\"");
    }

    e.symbol = sym;
    e.type = sym->type;
}


Type* testBinary(BinaryOp op, Type* leftType, Type* rightType) {
    switch (op) {
        case BinaryOp::Plus: {
            if (leftType == &Types::STRING_TYPE && rightType == &Types::STRING_TYPE) {
                return &Types::STRING_TYPE;
            }
            // Fallthrough to arithmetic cases.
        }
        case BinaryOp::Minus:
        case BinaryOp::Star:
        case BinaryOp::Slash: {
            if (leftType == &Types::INT_TYPE && rightType == &Types::INT_TYPE) {
                return &Types::INT_TYPE;
            } else if (leftType == &Types::INT_TYPE && rightType == &Types::DOUBLE_TYPE) {
                return &Types::DOUBLE_TYPE;
            } else if (leftType == &Types::DOUBLE_TYPE && rightType == &Types::INT_TYPE) {
                return &Types::DOUBLE_TYPE;
            } else if (leftType == &Types::DOUBLE_TYPE && rightType == &Types::DOUBLE_TYPE) {
                return &Types::DOUBLE_TYPE;
            }
            throw KMYCompileError("Invalid operand types for binary operator.");
        }



        case BinaryOp::Percent: {
            if (leftType != &Types::INT_TYPE) {
                throw KMYCompileError("Divident cannot be non-int.");
            }

            if (rightType != &Types::INT_TYPE) {
                throw KMYCompileError("Divisor cannot be non-int.");
            }

            // C-style: integer modulo only
            return &Types::INT_TYPE;
        }
        // TODO: Combine?
        case BinaryOp::LShift: {
            if (leftType == &Types::DOUBLE_TYPE || rightType == &Types::DOUBLE_TYPE) {
                throw KMYCompileError("Invalid operand.");
            }
            return &Types::INT_TYPE;
        }

        case BinaryOp::RShift: {
            if (leftType == &Types::DOUBLE_TYPE || rightType == &Types::DOUBLE_TYPE) {
                throw KMYCompileError("Invalid operand.");
            }
            return &Types::INT_TYPE;
        }

        case BinaryOp::BitAnd: {
            if (leftType == &Types::DOUBLE_TYPE || rightType == &Types::DOUBLE_TYPE) {
                throw KMYCompileError("Invalid operand.");
            }
            return &Types::INT_TYPE;
        }

        case BinaryOp::BitOr: {
            if (leftType == &Types::DOUBLE_TYPE || rightType == &Types::DOUBLE_TYPE) {
                throw KMYCompileError("Invalid operand.");
            }
            return &Types::INT_TYPE;
        }

        case BinaryOp::BitXor:{
            if (leftType == &Types::DOUBLE_TYPE || rightType == &Types::DOUBLE_TYPE) {
                throw KMYCompileError("Invalid operand.");
            }
            return &Types::INT_TYPE;
        }


        // ========================
        // LOGICAL
        // ========================

        // Following C style permissivity.
        case BinaryOp::LogicalAnd:
        case BinaryOp::LogicalOr:
            return &Types::BOOL_TYPE;

        // ========================
        // COMPARISON
        // ========================

        // Equality following modern language strictness.
        case BinaryOp::Greater:
        case BinaryOp::GreaterEqual:
        case BinaryOp::Less:
        case BinaryOp::LessEqual: {
            if (leftType != &Types::INT_TYPE && leftType != &Types::DOUBLE_TYPE) {
                throw KMYCompileError("Numeric values can only be compared.");
            }
            if (rightType != &Types::INT_TYPE && rightType != &Types::DOUBLE_TYPE) {
                throw KMYCompileError("Numeric values can only be compared.");
            }
            return &Types::BOOL_TYPE;
        }

        case BinaryOp::EqualEqual:
        case BinaryOp::NotEqual: {
            if (leftType != rightType) {
                throw KMYCompileError("Cannot compare different types.");
            }
            return &Types::BOOL_TYPE;
        }
        default:
            throw KMYCompileError("Unsupported binary operator");
    }
}


void Resolver::visit(BinaryExpr& e) {
    e.left->accept(*this);
    e.right->accept(*this);

    e.type = testBinary(e.op, e.left->type, e.right->type);
}

void Resolver::visit(UnaryExpr& e) {
    e.operand->accept(*this);

    if (e.operand->type != &Types::INT_TYPE) {
        throw std::runtime_error("Unary operator only supports int operand... for now.");
    }
    e.type = e.operand->type; // Int.
}

static bool isAssignable(Type* from, Type* to) {
    if (to == &Types::ANY_TYPE) {
        return true;
    }

    // HACK?
    if (from == &Types::ANY_TYPE) {
        return true;
    }
    
    if (from == &Types::INT_TYPE && to == &Types::DOUBLE_TYPE) {
        return true; // int can be assigned to double.
    }

    return from == to;
}

void Resolver::visit(Assignment& e) {
    assigning = true;
    e.left->accept(*this);
    assigning = false;

    Type* oldET = expectedType;
    expectedType = e.left->type;

    e.right->accept(*this);

    // Assignment doesn't change the type... right?
    e.type = e.left->type;

    expectedType = oldET;

    // std::cout << "In assign checking assignability of " << (int)(e.left->type->kind) << " and  " << (int)(e.right->type->kind) << "\n"; 

    if (!isAssignable(e.right->type, e.left->type)) {
        // TODO: Covariant and contravariant type checking.
        throw KMYCompileError(
            "Type mismatch in assignment from " 
            + typeToString(e.right->type) 
            + " to " 
            + typeToString(e.left->type)
        );
    }
}

void Resolver::visit(Index& e) {
    e.obj->accept(*this);

    if (e.obj->type->kind != TypeKind::ARRAY && e.obj->type != &Types::ANY_TYPE) {
        throw KMYCompileError("Only arrays can be indexed... for now.");
    }

    e.index->accept(*this);
    if (e.index->type != &Types::INT_TYPE && e.index->type != &Types::ANY_TYPE) {
        throw KMYCompileError("Index must be an integer.");
    }

    if (e.obj->type == &Types::ANY_TYPE) {
        e.type = &Types::ANY_TYPE;
    } else {
        e.type =
            static_cast<ArrayType*>(e.obj->type)->elementType;
    }
}

void Resolver::visit(Call& e) {
    e.func->accept(*this);
    if (e.func->type->kind != TypeKind::FUNCTION && e.func->type != &Types::ANY_TYPE) {
        throw KMYCompileError("Uncallable object.");
    }

    auto fnType = static_cast<FunctionType*>(e.func->type);

    e.type = fnType->returnType;

    // Call arity checks.
    // If function type doesnt convey param info, no checks are done.
    // Runtime checks will do that.

    if (!fnType->infoExists) {
        for (size_t i = 0; i < e.args.size(); ++i) {
            e.args[i]->accept(*this);
        }

        return;
    }

    int requiredCnt = 0;
    bool isVariadic = false;
    int totalParamCnt = fnType->info.size();

    size_t paramStartIdx = 0;
    for (auto& info : fnType->info) {
        if (!info.hasDefault && !info.implicitThis) {
            // Ignore implciit this as required cnt
            // Also need to ignore the first arg type check below.
            requiredCnt++;
        }
        if (info.implicitThis) {
            paramStartIdx++;
        }
        if (info.isVariadic) {
            isVariadic = true;
        }
    }

    // Min check
    if (e.args.size() < static_cast<size_t>(requiredCnt)) {
        throw KMYCompileError("Not enough arguments provided. required " + std::to_string(requiredCnt) + ", got " + std::to_string(e.args.size()));
    }

    // Max check
    if (!isVariadic && e.args.size() > totalParamCnt) {
        throw KMYCompileError("Too many arguments provided.");
    }

    
    for (size_t i = 0; i < e.args.size(); ++i) {
        e.args[i]->accept(*this);

        if (i < totalParamCnt) {
            if (!isAssignable(e.args[i]->type, fnType->info[paramStartIdx++].type)) {
                throw KMYCompileError("Argument type mismatch.");
            }
        } else {
            // Variadic arg
            if (!isAssignable(e.args[i]->type, fnType->info.back().type)) {
                throw KMYCompileError("Argument type mismatch.");
            }
        }
    }
}

void Resolver::visit(Get& e) {
    
    e.obj->accept(*this);
    std::cout << "In get, this guy's type kind in int: " << (int)(e.obj->type->kind) << "\n";
    
    if (e.obj->type->kind != TypeKind::STRUCTUAL && e.obj->type->kind != TypeKind::INSTANCE && e.obj->type != &Types::ANY_TYPE) {
        throw KMYCompileError(
            "Only record, or class instances can be accessed with dot operator. (Use \"::\" for enum or static members)"
        );
    }

    if (e.obj->type->kind == TypeKind::STRUCTUAL) {
        StructualType* recordType = static_cast<StructualType*>(e.obj->type);
        auto it = recordType->fieldTypes.find(e.name);
        if (it != recordType->fieldTypes.end()) {
            e.type = it->second;
            e.fieldIdx = recordType->layout[e.name];
            return;
        } 

        throw KMYCompileError("Field not found: " + e.name);

    } else if (e.obj->type->kind == TypeKind::INSTANCE) {
        InstanceType* aggType = static_cast<InstanceType*>(e.obj->type);

        {
            auto it = aggType->fieldMap.find(e.name);
            if (it != aggType->fieldMap.end()) {
                auto memberSym = it->second;
                e.type = memberSym->type;
                e.fieldIdx = memberSym->fieldOffset;
                return;
            } 
        }
            

        {
            auto it = aggType->methodMap.find(e.name);
            if (it != aggType->methodMap.end()) {
                auto memberSym = it->second;
                e.type = memberSym->type;
                // Used later for lowering
                e.resolvedMethod = true;
                e.methodIdx = memberSym->methodIdx;
                return;
            } 
        }

        // No constructor traverse.

        throw KMYCompileError("Property not found: " + e.name);
    }

    throw KMYCompileError("Get from any");
    // Fallback
    e.type = &Types::ANY_TYPE;
    e.fieldIdx = 9999;
}

void Resolver::visit(ScopeAccessExpr& e) {
    if (e.parts.size() < 2)
        throw KMYCompileError("Invalid scope access. This should not be even allowed here though...");

    // Step 1: resolve first part as type
    auto typeSym = resolveTypeSymbol(e.parts[0]);

    if (!typeSym)
        throw KMYCompileError("\"" + e.parts[0] + "\" is not a type symbol.");

    // Step 2: walk intermediate parts (if any)
    for (size_t i = 1; i < e.parts.size() - 1; ++i) {
        // TODO: Currently, :: can... now be nested!
        // BUT ONLY AGGREGATES CAN HAVE THEM.
        if (typeSym->type->kind == TypeKind::INSTANCE) {
            auto aggType = static_cast<InstanceType*>(typeSym->type);
            const std::string& currPart = e.parts[i];
            
            auto it = aggType->enumMap.find(currPart);
            if (it != aggType->enumMap.end()) {
                // You found it!
                typeSym = it->second;
            }
        } else {
            throw KMYCompileError("Only aggregates support nested ::");
        }
    }

    // AMBICIOUS TODO: IMPLEMENT "RUST" STYLE REFERENCE CHECKS! but optional for dev power
    // Heres a short poem i wrote for no reason ON A COMPILER
    
    /*
    hull
    your effort has gone null
    might be furious as a bull
    want some tool?
    to make the situation cool?
    or a wool?
    to learn what's bool? (WTF last line is ass)
    */ 

    // Step 3: handle FINAL part separately
    const std::string& last = e.parts.back();

    // TODO: Currently, the last MUST be an enum...
    if (typeSym->type->kind == TypeKind::ENUM) {
        auto enumType = static_cast<EnumType*>(typeSym->type);

        std::cout << "Checking enum " << typeSym->name << "\n";
        auto it = enumType->variantMap.find(last);
        if (it == enumType->variantMap.end()) {
            throw KMYCompileError("Enum value not found: " + last);
        }

        e.type = enumType;              // enum type
        e.accessIdx = it->second;       // enum variant index
        return;
    }

    throw KMYCompileError("Only enums supported for ends of :: right now");
}


void Resolver::visit(FunctionExpr& e) {
    Scope* old = currScope;

    currScope = e.scope; // scope created in Pass 1
   
    std::vector<Type*> paramTypes;
    std::vector<ParamTypeInfo> info;

    for (auto& param : e.params) {
        if (!param.symbol->type) {
            // This(^) check is present since param.symbol->type can be already defined
            // in which case, it should not be overriden.
            // Namely, implicit "this" parameter. 
            if (!param.type) {
                // No annotation means we don't know the type. Assume any.
                // We delegate this in runtime.
                param.symbol->type = &Types::ANY_TYPE;
    
                // If strict mode:
                // throw KMYCompileError("Parameter must have explicit type signature.");
            } else {
                std::cout << "param has annotation! but the symbol doesnt have it\n";
                param.symbol->type = typeSigToType(param.type);
                std::cout << "ok? so the error is after here?\n";
            }
        }

        paramTypes.push_back(param.symbol->type);

        if (param.defaultExists) {
            param.defaultValue->accept(*this);
            if (param.defaultValue->type != param.symbol->type) {
                throw KMYCompileError("Default value type mismatch.");
            }
        }
        
        // Param is now usable (prevents f(a = a) or f(a = b, b = 3) style errors.)
        param.symbol->available = true;

        info.push_back({param.defaultExists, param.isVariadic, param.implicitThis, param.symbol->type});
    }

    e.body->accept(*this);

    currScope = old;

    // IMPORTANT NOTE: SEMANTIC INFO IS LOST WHEN ANNOTATED.
    std::cout << "function return type annotation check\n";
    FunctionType* fnType = TypeInterner::getFunctionType(
        std::move(paramTypes),
        e.annotatedReturnType ? typeSigToType(e.annotatedReturnType) : &Types::ANY_TYPE
    );
    std::cout << "ok? so the error is NOT after here???????\n";

    fnType->info = std::move(info);
    fnType->infoExists = true;
    e.type = fnType;
    std::cout << "function np\n";
}

void Resolver::visit(ThisExpr& e) {
    if (!currentThis) {
        throw KMYCompileError("\"this\" used outside of method... :(");
    }
    std::cout << "Cthis is " << currentThis << std::endl;
    std::cout << "which refers to " << static_cast<InstanceType*>(currentThis->type)->name<< std::endl;

    e.symbol = currentThis;
    e.type = currentThis->type;
}

void Resolver::visit(NewExpr& e) {
    TypeSymbol* aggType = resolveTypeSymbol(e.typeName);
    
    // TODO: Separate instance and agg type...
    if (aggType->type->kind != TypeKind::INSTANCE) {
        throw KMYCompileError("\"new\" keyword applied to non aggregate (record / class).");
    }

    e.type = aggType->type;

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

void handleAnnotatedAndInferred(VarSymbol* sym, Type* annotated, Type* inferred) {
    std::cout << "annotated: " << typeToString(annotated)<< std::endl;
    std::cout << "inferred: " << typeToString(inferred) << std::endl;
    std::cout << "sym:" << sym << std::endl;
    printLog(LogLevel::DEBUG, "Handling annotated and inferred types\n");

    // CASE 1: annotated type exists
    if (annotated) {
        if (inferred) {
            if (!isAssignable(inferred, annotated)) {
                throw KMYCompileError(
                    "Type mismatch in assignment from " + typeToString(inferred) + " to " + typeToString(annotated)
                );
            }
            // Only functions have priority of inferred type over annotated type, since they may carry critical info like param defaults and variadicity that the annotated type doesn't convey.
            if (annotated->kind == TypeKind::FUNCTION) {
                sym->type = inferred;
                return;
            }
        }

        // You lose parameter defaults or vararg info.
        sym->type = annotated;
    }

    // CASE 2: no annotation → infer
    else {
        if (!inferred) {
            sym->type = &Types::UNINITIALISED;
            // throw KMYCompileError("Cannot infer type of uninitialised variable.");
        } else {
            sym->type = inferred;
        }

    }
}

void Resolver::visit(Let& s) {
    printLog(LogLevel::DEBUG, "Visiting let node for " + s.name + "\n");

    // Check pass 1 did correct job.
    assert(s.symbol);
    
    Type* annotated = nullptr;

    if (s.annotatedType) {
        annotated = typeSigToType(s.annotatedType);

        if (annotated == &Types::VOID_TYPE) {
            // TODO: if not strict mode, u ignore this. 
            throw KMYCompileError("Variable cannot be of type void.");
        }
    }

    Type* inferred = nullptr;

    Type* oldET = expectedType;
    if (annotated) { expectedType = annotated; }

    if (s.expr) {
        s.expr->accept(*this);
        inferred = s.expr->type;
    }

    // After visiting the expr, the symbol is now available to use.
    s.symbol->available = true;

    if (annotated) { expectedType = oldET; }

    handleAnnotatedAndInferred(s.symbol, annotated, inferred);
}

void Resolver::visit(Return& s) {
    if (currScope->depth <= 0) {
        // Not in a function
        KMYCompileError("Invalid return statement. Not in a function.");
    }

    if (s.expr)
        s.expr->accept(*this);
}

void Resolver::visit(Aggregate& s) {
    Scope* old = currScope;
    auto oldAgg = currentAggregate;

    currScope = s.scope;

    // Critical: make class type right after. Initially its inner types are empty.
    InstanceType* aggType = new InstanceType();
    aggType->name = s.name; // DEBUG

    if (!s.typeSymbol) {
        throw KMYCompileError("? agg type symbol where");
    }

    s.typeSymbol->type = aggType;
    currentAggregate = aggType;

    // Enums should be done first.
    // *****CRITICAL TODO: IN FACT, THESE SHOULD BE IN THE ORDER OF DECL!!!!!!!!!
    // MAKE just member vector and kind to just dispatch.
    // ALSO CONSIDER DOING VIRTUAL DISPATCH FOR ALL derived types instead of kind switches...
    // if possible tho.
    // 
    for (auto& member : s.enumMembers) {
        member.customEnum->accept(*this);

        aggType->enumMap[member.customEnum->name] = member.customEnum->typeSymbol;
    }
    
    std::cout << "Resolver: enum np\n";

    int offset = 0;
    for (auto& field : s.fieldMembers) {
        Type* inferred = nullptr;

        if (field.initialiser) {
            std::cout << "currthis: " << currentThis << std::endl;
            field.initialiser->accept(*this);
        }

        handleAnnotatedAndInferred(
            field.symbol,
            field.annotatedType ? typeSigToType(field.annotatedType) : nullptr,
            inferred
        );
    
        field.symbol->fieldOffset = offset++;
        aggType->fieldMap[field.name] = field.symbol;
    }

    std::cout << "Resolver: field np\n";

    
    int methodIdx = 0;
    auto oldThis = currentThis; // Do i need this?
    for (auto& member : s.methodMembers) {
        // HACK?: First parameter is ALWAYS implicit "this"
        currentThis = member.methodExpr->params[0].symbol;
        currentThis->type = currentAggregate;

        member.methodExpr->accept(*this);
        
        std::cout << "fnexpr visit done in method\n";
        member.symbol->type = member.methodExpr->type; 
        std::cout << "crash 1\n";
        member.symbol->methodIdx = methodIdx++;
        std::cout << "crash 2\n";
        aggType->methodMap[member.name] = member.symbol;
        std::cout << "???????\n";
    }

    std::cout << "Resolver: method np\n";

    for (auto& member : s.constructorMembers) {
        currentThis = member.initFuncExpr->params[0].symbol;
        currentThis->type = currentAggregate;

        member.initFuncExpr->accept(*this);
        // Constructor has no type...
        member.symbol->type = member.initFuncExpr->type;
        aggType->constructorVec.push_back(member.symbol);
    }

    std::cout << "Resolver: ctor np\n";

    

    std::cout << currentAggregate->name << "\n";
    // Field initialiser
    currentThis = s.fieldInitFunc->params[0].symbol;
    currentThis->type = currentAggregate;
    s.fieldInitFunc->accept(*this);

    std::cout << "Resolver: field init func np\n";

    currentThis = oldThis;
    currentAggregate = oldAgg;
    currScope = old;
}

void Resolver::visit(TypeAlias& s) {
    printLog(LogLevel::DEBUG, "Visiting typealias node for " + s.name + "\n");

    s.typeSymbol->type = typeSigToType(s.aliasingType);
}

void Resolver::visit(Enum& s) {
    printLog(LogLevel::DEBUG, "Visiting enum node for " + s.name + "\n");
    
    auto* enumType = new EnumType;
    
    // TODO: To type interner?
    int offset = 0;
    for (auto& str : s.variants) {
        enumType->variantMap[str] = offset++;
    }
    
    s.typeSymbol->type = enumType;
    
    printLog(LogLevel::DEBUG, "Enum offset is " + std::to_string(offset) + " for " + s.name + "\n");
}

void Resolver::visit(ExprStmt& s) {
    s.expr->accept(*this);
}