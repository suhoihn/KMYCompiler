#include "Resolver.hpp"

#include <iostream>
#include "../Core/errorhandler.hpp"
#include "../Utils/SymbolPrinter.hpp"
#include "../Utils/utils.hpp"
#include <assert.h>
#include "TypeHelpers.hpp"

Resolver::Resolver(
    FunctionExprPtr program,
    Scope* _globalScope
) : 
    program(program),
    globalScope(_globalScope),
    currScope(globalScope)
{}

void Resolver::resolve() {
    program->accept(*this);
}

VarSymbol* Resolver::lookupVarSymbol(const std::string& name) {
    Scope* scope = currScope;

    while (scope) {
        auto it = scope->values.find(name);
        
        if (it != scope->values.end()) {
            if (it->second->available) {
                return it->second;
            }
            return nullptr;
        }

        scope = scope->parent;
    }
    return nullptr; // Undefined variable.
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
            if (arrayType->fixedLength.has_value() && *arrayType->fixedLength != 0) {
                throw KMYCompileError("Empty array literal does not match fixed array length.");
            }
            e.type = arrayType;
            return;
        }
    }

    e.type = TypeInterner::getArrayType(baseType, e.elements.size());
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
    }

    e.type = TypeInterner::getStructualType(std::move(fieldTypes));
}

void Resolver::visit(Variable& e) {
    VarSymbol* sym = lookupVarSymbol(e.name);

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

        case BinaryOp::NullCoalesce:
            if (!leftType || leftType->kind != TypeKind::NULLABLE)
                throw KMYCompileError("Left operand of ?? must be nullable.");
            if (rightType != static_cast<NullableType*>(leftType)->innerType)
                throw KMYCompileError("Null-coalescing fallback has incompatible type.");
            return static_cast<NullableType*>(leftType)->innerType;

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

    if (e.op == UnaryOp::ForceUnwrap) {
        if (!e.operand->type || e.operand->type->kind != TypeKind::NULLABLE)
            throw KMYCompileError("!! requires a nullable operand.");
        e.type = static_cast<NullableType*>(e.operand->type)->innerType;
        return;
    } else if (e.op == UnaryOp::AddressOf) {
        if (!e.operand->isLValue())
            throw KMYCompileError("Address-of requires an lvalue operand.");
        e.type = TypeInterner::getPointerType(e.operand->type);
        return;
    } else if (e.op == UnaryOp::Dereference) {
        if (!e.operand->type || e.operand->type->kind != TypeKind::POINTER)
            throw KMYCompileError("Dereference applied to non-pointer!!!");
        auto* ptrType = static_cast<PointerType*>(e.operand->type);
        if (ptrType->pointee == &Types::ANY_TYPE)
            throw KMYCompileError("Cannot dereference any*; first assign it to a typed pointer.");
        if (ptrType->pointee == &Types::VOID_TYPE)
            throw KMYCompileError("Cannot dereference void*.");
        e.type = ptrType->pointee;
        return;
    } else if (e.operand->type != &Types::INT_TYPE) {
        throw std::runtime_error("Unary operator only supports int operand... for now.");
    }
    e.type = e.operand->type; // Int.
}

static bool isAssignable(Type* from, Type* to) {
    if (to == &Types::ANY_TYPE) {
        return true;
    }

    // HACK?
    //if (from == &Types::ANY_TYPE) {
    //    return true;
    //}
    
    if (from == &Types::INT_TYPE && to == &Types::DOUBLE_TYPE) {
        return true; // int can be assigned to double.
    }

    if (from && to && from->kind == TypeKind::POINTER && to->kind == TypeKind::POINTER) {
        auto* source = static_cast<PointerType*>(from);
        auto* target = static_cast<PointerType*>(to);
        // A malloc result is opaque until the programmer names the pointee.
        // This is an unsafe conversion: the compiler cannot verify byte count.
        return source->pointee == &Types::ANY_TYPE ||
               target->pointee == &Types::ANY_TYPE ||
               source->pointee == target->pointee;
    }

    // Arrays are pointer-backed at runtime, but their semantic type also
    // carries an optional compile-time length.  An unsized T[] target accepts
    // any fixed-length T[N] source; a fixed-length target requires the same
    // length.  This keeps `int[4]` usable wherever `int[]` is expected.
    if (from && to && from->kind == TypeKind::ARRAY && to->kind == TypeKind::ARRAY) {
        auto* sourceArray = static_cast<ArrayType*>(from);
        auto* targetArray = static_cast<ArrayType*>(to);

        if (!isAssignable(sourceArray->elementType, targetArray->elementType)) {
            return false;
        }

        if (!targetArray->fixedLength.has_value()) {
            return true;
        }

        return sourceArray->fixedLength.has_value()
            && sourceArray->fixedLength == targetArray->fixedLength;
    }

    if (to && to->kind == TypeKind::NULLABLE) {
        // Nullable targets accept either null or the wrapped value type.
        // A nullable source is compatible only when its inner type can be
        // assigned to this target's inner type; null never flows into a
        // non-nullable target through this rule.
        auto* nullableTo = static_cast<NullableType*>(to);
        if (from == &Types::NULL_TYPE) return true;
        if (from && from->kind == TypeKind::NULLABLE) {
            return isAssignable(static_cast<NullableType*>(from)->innerType, nullableTo->innerType);
        }
        return isAssignable(from, nullableTo->innerType);
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

    // Call arity checks. Plain function signatures (including builtins)
    // still carry paramTypes even when they have no default/variadic metadata.

    if (!fnType->infoExists) {
        if (e.args.size() != fnType->paramTypes.size()) {
            throw KMYCompileError("Wrong number of arguments in function call.");
        }
        for (size_t i = 0; i < e.args.size(); ++i) {
            e.args[i]->accept(*this);
            if (!isAssignable(e.args[i]->type, fnType->paramTypes[i])) {
                throw KMYCompileError("Argument type mismatch.");
            }
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

    // Step 1: lookup first part as type
    auto typeSym = lookupTypeSymbol(currScope, e.parts[0]);

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

    // Param types, return type, and eventually function signature are declared in pass 2.
    for (auto& param : e.params) {

        if (param.defaultExists) {
            param.defaultValue->accept(*this);
            if (param.defaultValue->type != param.symbol->type) {
                throw KMYCompileError("Default value type mismatch.");
            }
        }
    }

    e.body->accept(*this);

    currScope = old;
}

void Resolver::visit(ThisExpr& e) {
    // In pass 2.
    /*
    if (!currentThis) {
        throw KMYCompileError("\"this\" used outside of method... :(");
    }

    e.symbol = currentThis;
    e.type = currentThis->type;
    */
}

void Resolver::visit(NewExpr& e) {
    if (e.arrayType) {
        e.type = typeSigToType(currScope, e.arrayType);
        if (e.type->kind != TypeKind::ARRAY) {
            throw KMYCompileError("Array allocation requires an array type.");
        }
        auto* arrayType = static_cast<ArrayType*>(e.type);
        if (!arrayType->fixedLength.has_value() && !e.arraySize) {
            throw KMYCompileError("Array allocation requires a size, e.g. new int[5] or new int[count].");
        }
        if (e.arraySize) {
            e.arraySize->accept(*this);
            if (!e.arraySize->type || e.arraySize->type->kind != TypeKind::INT) {
                throw KMYCompileError("Array size expression must have type int.");
            }
        }
        return;
    }

    TypeSymbol* aggType = lookupTypeSymbol(currScope, e.typeName);
    
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

    // Check pass 1 did correct job.
    assert(s.symbol);
    
    Type* annotated = nullptr;

    if (s.annotatedType) {
        annotated = typeSigToType(currScope, s.annotatedType);

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

    // Aggregate type is defined in pass 2.
    assert(s.typeSymbol);
    assert(s.typeSymbol->type);

    auto aggType = static_cast<InstanceType*>(s.typeSymbol->type);
    currentAggregate = aggType;

    // Enums should be done first.
    // *****CRITICAL TODO: IN FACT, THESE SHOULD BE IN THE ORDER OF DECL!!!!!!!!!
    // MAKE just member vector and kind to just dispatch.
    // ALSO CONSIDER DOING VIRTUAL DISPATCH FOR ALL derived types instead of kind switches...
    // if possible tho.
    // 
    
    // No need to visit enum again for resolving.

    int offset = 0;
    for (auto& field : s.fieldMembers) {
        Type* inferred = nullptr;

        if (field.initialiser) {
            field.initialiser->accept(*this);
        }

        handleAnnotatedAndInferred(
            field.symbol,
            field.annotatedType ? typeSigToType(currScope, field.annotatedType) : nullptr,
            inferred
        );
    
        field.symbol->fieldOffset = offset++;
    }


    
    int methodIdx = 0;
    auto oldThis = currentThis; // Do i need this?
    for (auto& member : s.methodMembers) {
        // HACK?: First parameter is ALWAYS implicit "this"
        currentThis = member.methodExpr->params[0].symbol;
        currentThis->type = currentAggregate;

        member.methodExpr->accept(*this);
        
        member.symbol->type = member.methodExpr->type; 
        member.symbol->methodIdx = methodIdx++;
    }


    for (auto& member : s.constructorMembers) {
        currentThis = member.initFuncExpr->params[0].symbol;
        currentThis->type = currentAggregate;

        member.initFuncExpr->accept(*this);
    }


    

    // Field initialiser
    currentThis = s.fieldInitFunc->params[0].symbol;
    currentThis->type = currentAggregate;
    s.fieldInitFunc->accept(*this);


    currentThis = oldThis;
    currentAggregate = oldAgg;
    currScope = old;
}

void Resolver::visit(TypeAlias& s) {
    // In pass 2.
    // s.typeSymbol->type = typeSigToType(currScope, s.aliasingType);
}

void Resolver::visit(Enum& s) {
    
    // Type already built in pass 2.
    /*
    auto* enumType = new EnumType;
    
    // TODO: To type interner?
    int offset = 0;
    for (auto& str : s.variants) {
        enumType->variantMap[str] = offset++;
    }
    
    s.typeSymbol->type = enumType;
    */

}

void Resolver::visit(ExprStmt& s) {
    s.expr->accept(*this);
}
