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

TypeSymbol* Resolver::resolveTypeSymbol(const std::string& name) {
    Scope* scope = currScope;
    std::cout << "[DEBUG] Resolving type symbol: " << name << std::endl;
    std::cout << scope << std::endl;

    while (scope) {
        std::cout << scope << std::endl;
        auto it = scope->types.find(name);

        if (it != scope->types.end()) {
            return it->second;
        }

        scope = scope->parent;
    }
    std::cout << "[DEBUG] Type symbol not found: " << name << std::endl;
    return nullptr; // Undefined type name.
}

Type* Resolver::typeSigToType(const TypeNodePtr& type) {
    switch (type->kind) {
        case TypeNodeKind::NAMED: {
            const NamedTypeNode& named = static_cast<NamedTypeNode&>(*type);
            if (named.name == "int") {
                return &Types::INT_TYPE;
            } else if (named.name == "double") {
                return &Types::DOUBLE_TYPE;
            } else if (named.name == "bool") {
                return &Types::BOOL_TYPE;
            } else if (named.name == "string") {
                return &Types::STRING_TYPE;
            } else if (named.name == "null") {
                return &Types::NULL_TYPE;
            } else if (named.name == "void") {
                return &Types::VOID_TYPE;
            } else if (named.name == "any") {
                return &Types::ANY_TYPE;
            } else {
                auto sym = resolveTypeSymbol(named.name);
                if (!sym) {
                    throw KMYCompileError("Unknown type name: " + named.name);
                }
                return sym->type;
            }
        }

        // TODO: interning (shared ptr for distinct types.)
        case TypeNodeKind::FUNCTION: {
            const FunctionTypeNode& named = static_cast<FunctionTypeNode&>(*type);
            std::vector<Type*> paramTypes;
            for (const auto& param : named.params) {
                paramTypes.push_back(typeSigToType(param));
            }
            Type* returnType = typeSigToType(named.returnType);
            // TODO: Raw. new. memory leek guaraneed.
            // No param info.
            return new FunctionType(paramTypes, returnType);
        }

        case TypeNodeKind::RECORD: {
            const RecordTypeNode& named = static_cast<RecordTypeNode&>(*type);
            std::unordered_map<std::string, Type*> fieldTypes;
            std::unordered_map<std::string, int> layout;

            for(auto& pair : named.paramTypePairs) {
                fieldTypes[pair.first] = typeSigToType(pair.second);
                layout[pair.first] = layout.size();
                std::cout << layout.size() << std::endl;
            }

            return new AggregateType(std::move(fieldTypes), std::move(layout));
        }
    }

    throw KMYCompileError("Unknown type node kind.");
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
    for (auto& elem : e.elements) {
        elem->accept(*this);
        continue;

        if (baseType) {
            if (baseType != elem->type && elem->type != &Types::ANY_TYPE) {
                // TODO
                //throw KMYCompileError("Unmatched array element type.");
            }
            // TODO...
        } else {
            baseType = elem->type;
        }
    }
    // TODO: Disgusting memory leak.
    // nullptr type for array means empty array.
    if (e.elements.empty()) {
        if (expectedType->kind != TypeKind::ARRAY) {
            throw KMYCompileError("Expected array type!!!");
        }
        auto arrayType = static_cast<ArrayType*>(expectedType);
        e.type = arrayType;
        return;
    }

    e.type = new ArrayType(baseType);
}

void Resolver::visit(RecordLiteral& e) {
    std::unordered_map<std::string, Type*> fieldTypes;

    for (auto& [fieldName, value] : e.fields) {
        value->accept(*this);
        fieldTypes[fieldName] = value->type;
    }

    for (auto& [name, slot] : e.layout) {
        std::cout << "name: " << name << "slot: " << slot << std::endl;
    }

    e.type = new AggregateType(std::move(fieldTypes), std::move(e.layout));
}

void Resolver::visit(Variable& e) {
    SymbolPtr sym = resolveSymbol(e.name);
    std::cout << "[DEBUG] Resolved symbol: " << sym << std::endl;

    if (!sym) {
        throw KMYCompileError(
            "Undefined variable \"" + e.name + "\""
        );
    }

    e.symbol = sym;
    e.type = sym->type;
}


Type* testBinary(BinaryOp op, Type* leftType, Type* rightType) {
    switch (op) {
        case BinaryOp::Plus:
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
        }



        case BinaryOp::Percent: {
            if (leftType == &Types::DOUBLE_TYPE) {
                throw std::runtime_error("Divident cannot be double.");
            }

            if (rightType == &Types::DOUBLE_TYPE) {
                throw std::runtime_error("Divisor cannot be double.");
            }

            // C-style: integer modulo only
            return &Types::INT_TYPE;
        }
        // TODO: Combine?
        case BinaryOp::LShift: {
            if (leftType == &Types::DOUBLE_TYPE || rightType == &Types::DOUBLE_TYPE) {
                throw std::runtime_error("Invalid operand.");
            }
            return &Types::INT_TYPE;
        }

        case BinaryOp::RShift: {
            if (leftType == &Types::DOUBLE_TYPE || rightType == &Types::DOUBLE_TYPE) {
                throw std::runtime_error("Invalid operand.");
            }
            return &Types::INT_TYPE;
        }

        case BinaryOp::BitAnd: {
            if (leftType == &Types::DOUBLE_TYPE || rightType == &Types::DOUBLE_TYPE) {
                throw std::runtime_error("Invalid operand.");
            }
            return &Types::INT_TYPE;
        }

        case BinaryOp::BitOr: {
            if (leftType == &Types::DOUBLE_TYPE || rightType == &Types::DOUBLE_TYPE) {
                throw std::runtime_error("Invalid operand.");
            }
            return &Types::INT_TYPE;
        }

        case BinaryOp::BitXor:{
            if (leftType == &Types::DOUBLE_TYPE || rightType == &Types::DOUBLE_TYPE) {
                throw std::runtime_error("Invalid operand.");
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
        case BinaryOp::EqualEqual:
        case BinaryOp::NotEqual:
        case BinaryOp::Greater:
        case BinaryOp::GreaterEqual:
        case BinaryOp::Less:
        case BinaryOp::LessEqual: {
            if (leftType != rightType) {
                throw std::runtime_error("Cannot compare different types.");
            }
            return &Types::BOOL_TYPE;
        }
        default:
            throw std::runtime_error("Unsupported binary operator");
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
    
    if (from == &Types::INT_TYPE && to == &Types::DOUBLE_TYPE) {
        return true; // int can be assigned to double.
    }

    return from == to;
}

void Resolver::visit(Assignment& e) {
    e.left->accept(*this);

    Type* oldET = expectedType;
    expectedType = e.left->type;

    e.right->accept(*this);

    expectedType = oldET;

    std::cout << "In assign checking assignability of " << (int)(e.left->type->kind) << " and  " << (int)(e.right->type->kind) << "\n"; 

    if (!isAssignable(e.right->type, e.left->type)) {
        // TODO: Covariant and contravariant type checking.
        throw KMYCompileError("Type mismatch in assignment.");
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

    std::cout << "1!!\n";
    auto fnType = static_cast<FunctionType*>(e.func->type);

    e.type = fnType->returnType;
    std::cout << "call return type " << e.type << "\n";
    std::cout << "is it any? ----> " << (int)(e.type == &Types::ANY_TYPE) << "\n";

    // Call arity checks.
    // If function type doesnt convey param info, no checks are done.
    // Runtime checks will do that.

    if (!fnType->infoExists)
        return;

    int requiredCnt = 0;
    bool isVariadic = false;
    int totalParamCnt = fnType->info.size();

    for (auto& info : fnType->info) {
        if (!info.hasDefault) {
            requiredCnt++;
        }
        if (info.isVariadic) {
            isVariadic = true;
        }
    }

    std::cout << "2!!\n";
    // Min check
    if (e.args.size() < static_cast<size_t>(requiredCnt)) {
        throw KMYCompileError("Not enough arguments provided.");
    }

    // Max check
    if (!isVariadic && e.args.size() > totalParamCnt) {
        throw KMYCompileError("Too many arguments provided.");
    }

    for (size_t i = 0; i < e.args.size(); ++i) {
        e.args[i]->accept(*this);

        if (i < totalParamCnt) {
            if (e.args[i]->type != fnType->info[i].type) {
                throw KMYCompileError("Argument type mismatch.");
            }
        } else {
            // Variadic args.
            if (e.args[i]->type != fnType->info.back().type) {
                throw KMYCompileError("Argument type mismatch.");
            }
        }
    }
}

void Resolver::visit(Get& e) {
    e.obj->accept(*this);
    if (e.obj->type->kind != TypeKind::AGGREGATE && e.obj->type->kind != TypeKind::INSTANCE && e.obj->type != &Types::ANY_TYPE) {
        throw KMYCompileError("Only record or class instances can be accessed with dot operator.");
    }

    std::cout << "In get, this guy's type kind in int: " << (int)(e.obj->type->kind) << "\n";
    
    // HACK!!!
    if (e.obj->type->kind == TypeKind::AGGREGATE) {
        AggregateType* recordType = static_cast<AggregateType*>(e.obj->type);
        auto it = recordType->fieldTypes.find(e.name);
        if (it == recordType->fieldTypes.end()) {
            throw KMYCompileError("Field not found: " + e.name);
        }
        e.type = it->second;
        e.fieldIdx = recordType->layout[e.name];
        return;

    } else if (e.obj->type->kind == TypeKind::INSTANCE) {
        throw KMYCompileError("Not yet bozo.");
    }

    // Fallback
    e.type = &Types::ANY_TYPE;
    e.fieldIdx = 9999;
}

void Resolver::visit(FunctionExpr& e) {
    Scope* old = currScope;

    currScope = e.scope; // scope created in Pass 1

    // Debug.
    for (const auto& [name, sym] : currScope->symbols) {
        std::cout << "  " << name << " (mutable: " << sym->isMutable << ")\n";
    }
   
    std::vector<Type*> paramTypes;
    std::vector<ParamTypeInfo> info;

    for (auto& param : e.params) {
        if (!param.type) {
            // No annotation means we don't know the type. Assume any.
            // We delegate this in runtime.
            param.symbol->type = &Types::ANY_TYPE;

            // If strict mode:
            // throw KMYCompileError("Parameter must have explicit type signature.");
        } else {
            param.symbol->type = typeSigToType(param.type);
        }

        paramTypes.push_back(param.symbol->type);

        if (param.defaultExists) {
            param.defaultValue->accept(*this);
            if (param.defaultValue->type != param.symbol->type) {
                throw KMYCompileError("Default value type mismatch.");
            }
        }

        info.push_back({param.defaultExists, param.isVariadic, param.symbol->type});
    }

    e.body->accept(*this);

    currScope = old;

    if (e.annotatedReturnType) {
        std::cout << "this guy has return sig" << std::endl;
    }

    e.type = new FunctionType(
        std::move(paramTypes),
        std::move(info),
        e.annotatedReturnType ? typeSigToType(e.annotatedReturnType) : &Types::ANY_TYPE
    );
}

void Resolver::visit(ThisExpr& e) {
    if (!currentThis) {
        throw KMYCompileError("\"this\" used outside of method... :(");
    }

    e.symbol = currentThis;
    e.type = currentThis->type;
}

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
    Type* annotated = nullptr;

    if (s.annotatedType) {
        annotated = typeSigToType(s.annotatedType);

        if (annotated == &Types::VOID_TYPE) {
            // TODO: if strict mode, u ignore this. 
            throw KMYCompileError("Variable cannot be of type void.");
        }
    }

    Type* inferred = nullptr;

    if (s.expr) {
        s.expr->accept(*this);
        inferred = s.expr->type;
        std::cout << "I have expr.\n";
    }

    std::cout << s.expr << std::endl;
    std::cout << annotated << std::endl;
    std::cout << inferred << std::endl;

    // CASE 1: annotated type exists
    if (annotated) {
        if (inferred) {
            if (!isAssignable(inferred, annotated)) {
                throw KMYCompileError("Type mismatch in initializer.");
            }
            s.symbol->type = inferred;
            return;
        }

        // You lose parameter defaults or vararg info.
        s.symbol->type = annotated;
    }

    // CASE 2: no annotation → infer
    else {
        if (!inferred) {
            throw KMYCompileError("Cannot infer type of uninitialised variable.");
        }

        s.symbol->type = inferred;
    }
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
    AggregateType* classType = new AggregateType();
    s.typeSymbol->type = classType;
    currentAggregate = classType;

    for (auto& field : s.fieldMembers) {
        Type* fieldType;

        if (field.annotatedType) {
            // Explicit annotation
            fieldType = typeSigToType(field.annotatedType);
        }
        else if (field.initialiser) {
            // Infer from initialiser
            field.initialiser->accept(*this);
            fieldType = field.initialiser->type;
        } else {
            fieldType = &Types::ANY_TYPE;
            // OR throw error...?
        }

        classType->fieldTypes[field.name] = fieldType;
        classType->layout[field.name] = classType->layout.size();

        field.symbol->type = fieldType;
    }

    auto oldThis = currentThis;
    currentThis = std::make_shared<Symbol>("this", false);
    currentThis->type = currentAggregate;

    for (auto& member : s.methodMembers) {
        member.methodExpr->accept(*this);
        member.symbol->type = member.methodExpr->type; 
    }
    
    currentThis = oldThis;
    currentAggregate = oldAgg;
    currScope = old;
}

void Resolver::visit(TypeAlias& s) {
    s.typeSymbol->type = typeSigToType(s.annotatedType);
}

void Resolver::visit(ExprStmt& s) {
    s.expr->accept(*this);
}