#include "interpreter.hpp"

#include <cmath>
#include "Ast.hpp"
#include "tokens.hpp"
#include "iterator.hpp"

struct BreakException {};
struct ContinueException {};
struct ReturnException {
    Value value;

    ReturnException(const Value& v) : value(v) {}
};

Value Interpreter::getResult(ExprPtr expr) {
    expr->accept(*this);
    return result;
}

Interpreter::Interpreter() {
    global = new Env();
    env = global;
}

void Interpreter::initBuiltins() {
    global->define("append", NativeFn(
        [](const std::vector<Value>& args) -> Value {
            auto arr = std::get<Array>(args[0].data);
            arr->push_back(args[1]);
            return nullptr;
        }
    ));

    global->define("len", NativeFn(
        [](const std::vector<Value>& args) -> Value {
            auto arr = std::get<Array>(args[0].data);
            return static_cast<int>(arr->size());
        }
    ));

    global->define("pop", NativeFn(
        [](const std::vector<Value>& args) -> Value {
            auto arr = std::get<Array>(args[0].data);
            int index = std::get<int>(args[1].data);
            auto& vec = *arr;

            if (vec.empty()) {
                throw runtime_error("pop from empty array");
            }
            if (index < 0 || index >= vec.size()) {
                throw runtime_error("invalid pop index");
            }
            Value v = vec.at(index);
            vec.erase(vec.begin() + index);
            return v;
        }
    ));
}

void Interpreter::interpret(const std::vector<StmtPtr>& stmts) {
    initBuiltins();
    for (auto& stmt : stmts) {
        stmt->accept(*this);
    }
}

void Interpreter::printEnv() const {
    global->print();
}

// -----Expressions visit
void Interpreter::visit(Literal& e) {
    result = std::visit([](auto&& arg) -> Value {
        return Value(arg);
    }, e.value);
};

void Interpreter::visit(ArrayLiteral& e) {
    auto arr = std::vector<Value>();

    for (auto& elem : e.elements) {
        arr.push_back(getResult(elem));
    }

    result = Value(
        std::make_shared<ArrayObj>(std::move(arr))
    );
}

void Interpreter::visit(RecordLiteral& e) {
    RecordObj rec;

    for (const auto& [key, value] : e.fields) {
        rec.fields[key] = getResult(value);
    }

    result = Value(std::make_shared<RecordObj>(rec));
}

void Interpreter::visit(Variable& e) {
    result = env->get(e.name);
}

void Interpreter::visit(BinaryExpr& e) {
    result = applyBinary(e.op, getResult(e.left), getResult(e.right));
}

void Interpreter::visit(UnaryExpr& e) {
    result = applyUnary(e.op, getResult(e.operand));
}


static bool isCompound(AssignmentOp op) {
    return op != AssignmentOp::Assign;
}

void Interpreter::visit(Assignment& e) {
    Value right = getResult(e.right);

    // Case 1: variable assignment
    if (auto* var = dynamic_cast<Variable*>(e.left.get())) {
        // cout << "[DEBUG] Assigning " << var->name << " = " << val.toString() << std::endl;

        if (isCompound(e.op)) {
            var->accept(*this);
            Value left = result;
            right = applyBinary(compoundToBinaryOp(e.op), left, right);
        }
        env->assign(var->name, right);
        result = right;
        return;
    }

    // Case 2: array index assignment
    if (auto* index = dynamic_cast<Index*>(e.left.get())) {

        Value obj = getResult(index->obj);
        Value idx = getResult(index->index);

        if (!checkObjType(obj, ObjKind::Array) && !checkObjType(obj, ObjKind::Record)) {
            throw std::runtime_error("In assignment, you indexed something that's not an array or a record.");
        }

        if (checkObjType(obj, ObjKind::Array)) {
            if (!std::holds_alternative<int>(idx.data)) {
                throw std::runtime_error("Array index must be an int");
            }

            ArrayPtr arr = std::static_pointer_cast<ArrayObj>(
                std::get<ObjectPtr>(obj.data)
            );
    
            int i = std::get<int>(idx.data);
    
            if (arr->array.size() == 0 || i < 0 || i >= (int)(arr->array.size())) {
                throw std::runtime_error("Index out of bounds");
            }
    
            if (isCompound(e.op)) {
                Value left = (arr->array)[i];
                right = applyBinary(compoundToBinaryOp(e.op), left, right);
            }
            (arr->array )[i] = right;
            
        } else if (checkObjType(obj, ObjKind::Record)) {
            if (auto *str = std::get_if<std::string>(&idx.data)) {
                auto rec = std::static_pointer_cast<RecordObj>(
                    std::get<ObjectPtr>(obj.data)
                );
                if (rec->fields.count(*str)) {
                    if (isCompound(e.op)) {
                        Value left = rec->fields.at(*str);
                        right = applyBinary(compoundToBinaryOp(e.op), left, right);
                    }
                    //cout << "Assigning with [] in obj.\n";
                }
                // TODO()?: if a["x"] and a.x doesn't exists, it makes new one. TODO()?
                rec->fields[*str] = right;
                    
            } else {
                throw std::runtime_error("Object index is not string.");
            }
        }
        result = right;
        return;
    }
    else if (auto* get = dynamic_cast<Get*>(e.left.get())) {
        Value objVal = getResult(get->obj);

        if (!checkObjType(objVal, ObjKind::Record)) {
            throw std::runtime_error("Cannot assign property on non-object.");
        }

        auto rec = std::static_pointer_cast<RecordObj>(
            std::get<ObjectPtr>(objVal.data)
        );

        // Assign field
        if (isCompound(e.op)) {
            if (!rec->fields.count(get->name)) {
                throw std::runtime_error("Undefined property " + get->name);
            }
            Value left = rec->fields.at(get->name);
            right = applyBinary(compoundToBinaryOp(e.op), left, right);
        }

        // Important: This can create a new field if doesn't exist!
        rec->fields[get->name] = right;
        result = right;

        return;
    }


    throw std::runtime_error("Not assignable");       
}


Value Interpreter::visit(Index& e) {
    Value obj = getResult(e.obj);
    Value idx = getResult(e.index);

    // Record
    if (auto *rec = std::get_if<ObjectPtr>(&obj.data)) {
        if (auto *str = std::get_if<std::string>(&idx.data)) {
            if ((*rec)->fields.count(*str)) {
                return (*rec)->fields.at(*str);
            }
            throw std::runtime_error("Property " + *str + " not found in the object.");
        }

        throw runtime_error("Object indexing must be string. Actually this is TODO().");
    }

    // Non-record
    if (!std::holds_alternative<int>(idx.data)) {
        throw runtime_error("Index must be int.");
    }

    int i = std::get<int>(idx.data);

    if (auto *arr = std::get_if<Array>(&obj.data)) {
        if (!(*arr) || i < 0 || i >= (int)((*arr)->size()))
            throw runtime_error("Index out of bounds");

        return (**arr)[i];
    }

    if (auto str = std::get_if<std::string>(&obj.data)) {
        if (i < 0 || i >= (int)(str->size()))
            throw runtime_error("Index out of bounds");

        return Value(std::string(1, (*str)[i]));
    }

    throw runtime_error("Value is not indexable");
}

Value Interpreter::visit(Call& e) {
    Value callee = e.func->accept(*this);
    
    // std::get_if<T>(&variant) returns a pointer to the contained T if active,
    // otherwise nullptr.
    if (auto *fn = std::get_if<NativeFn>(&callee.data)) {
        vector<Value> args;
        for (auto& arg : e.args) {
            args.push_back(arg->accept(*this));
        }
        return Value((*fn)(args));
    }
    
    FunctionPtr fn;
    if (std::holds_alternative<FunctionPtr>(callee.data) ) {
        fn = std::get<FunctionPtr>(callee.data);
    } else if (std::holds_alternative<BoundFunction>(callee.data)) {
        fn = std::get<BoundFunction>(callee.data).fn;
    } else {
        throw runtime_error("Not callable.");
    }

    // It is sure that the order is norm, default, vararg
    bool hasVararg = !fn->params.empty() && fn->params.back().isVariadic;
    size_t providedArgCnt = e.args.size();
    size_t totalParamsCnt = fn->params.size();
    size_t normalCount = hasVararg ? totalParamsCnt - 1 : totalParamsCnt;
    
    int requiredArgCnt = 0;
    for (auto& param : fn->params) {
        requiredArgCnt += static_cast<int>(!param.defaultExists && !param.isVariadic);
    }

    //cout << "P: " << providedArgCnt << " | R: " << requiredArgCnt << "\n";

    if (providedArgCnt < requiredArgCnt) // requiredArgCnt stores minimum arg needed.
        throw runtime_error("Too few arguments");

    if (!hasVararg && providedArgCnt > totalParamsCnt)
        throw runtime_error("Too many arguments");

    // PRE: providedArgCnt >= requiredArgCnt
    
    
    // Creates a local environment with parent = (current env).
    Env local(env);
    
    // EnvGuard sets env=local. (Now, env-> calls are connected to local)
    // If local is destroyed (when this function ends),
    // env = (backed up current env).
    EnvGuard guard(env, &local);

    //cout << "Env logic ok\n";

    if (auto *bound = std::get_if<BoundFunction>(&callee.data)) {
        // bind args, run function
        // cout << "bound function\n";
        env->define("this", Value(bound->receiver));
    }
    

    for (size_t i = 0; i < normalCount; ++i) {
        Value v = (i < providedArgCnt)
            ? e.args[i]->accept(*this)
            : fn->params[i].defaultValue->accept(*this);

        v.isMutable = fn->params[i].isMutable;
        env->define(fn->params[i].name, v);
    }

    if (hasVararg) {
        // TODO: const in vararg is very vague...
        // const array? const items?
        Array extraArgs = make_shared<vector<Value>>();
        const Parameter& vararg = fn->params.back();

        for (size_t i = normalCount; i < providedArgCnt; ++i) {
            Value v = e.args[i]->accept(*this);
            v.isMutable = vararg.isMutable;
            extraArgs->push_back(v);
        }

        env->define(vararg.name, Value(extraArgs));
    }

    try {
        //cout << "Accepted\n";
        fn->body->accept(*this);
    } catch (ReturnException& ret) {
        return ret.value;
    }

    return Value(nullptr);
}

Value Interpreter::visit(Get& e) {
    Value objVal = e.obj->accept(*this);

    if (!std::holds_alternative<ObjectPtr>(objVal.data)) {
        throw runtime_error("You can't get from non-object.");
    }

    auto obj = std::get<ObjectPtr>(objVal.data);

    // 1. INSTANCE FIELDS FIRST
    auto fieldIt = obj->fields.find(e.name);
    if (fieldIt != obj->fields.end()) {
        Value v = fieldIt->second;
        if (auto* fn = std::get_if<FunctionPtr>(&v.data)) {
            return BoundFunction{*fn, obj};
        }
        return v;
    }

    // 2. CLASS METHODS
    if (obj->cls) {
        auto methodIt = obj->cls->methods.find(e.name);

        if (methodIt != obj->cls->methods.end()) {
            BoundFunction bf;
            bf.fn = methodIt->second;
            bf.receiver = obj;

            return Value(bf);
        }
    }

    throw runtime_error("Undefined property '" + e.name + "'");
}

Value Interpreter::visit(FunctionExpr& e) {
    return make_shared<Function>(e.params, e.body);
}

Value Interpreter::visit(ThisExpr& e) {
    if (!env->has("this")) {
        throw runtime_error("Cannot use 'this' here. Only inside objects dude.");
    }
    return env->get("this");
}

Value Interpreter::visit(NewExpr& e) {
    Value v = env->get(e.typeName);
    if (!holds_alternative<ClassPtr>(v.data)) {
        throw runtime_error("new applied to non-class.");
    }
    ClassPtr cls = std::get<ClassPtr>(v.data);

    // TODO: initialiser logic here.

    return make_shared<Object>(cls->fieldDefaults, cls);
}

// Statements
void Interpreter::visit(Print& s) {
    Value v = getResult(s.expr);
    std::cout << v.toString() << std::endl;
}

void Interpreter::visit(If& s) {
    Value cond = getResult(s.condition);

    if (isTruthy(cond)) {
        s.thenbranch->accept(*this);
    } else if (s.elsebranch) {
        s.elsebranch->accept(*this);
    }
}

void Interpreter::visit(While& s) {
    while (true) {
        Value cond = getResult(s.condition);

        if (!isTruthy(cond)) break;

        try {
            s.body->accept(*this);
        } catch (BreakException&) {
            // FIXME: This is broken.
            break;
        } catch (ContinueException&) {
            continue;
        }
    }
}

void Interpreter::visit(Block& s) {
    // Creates a local environment with parent = (current env).
    Env local(env);
    
    // EnvGuard sets env=local. (Now, env-> calls are connected to local)
    // If local is destroyed (when this function ends),
    // env = (backed up current env).
    EnvGuard guard(env, &local);

    for (auto& stmt : s.statements) {
        stmt->accept(*this);
    }
}

void Interpreter::visit(Break& s) { 
    throw BreakException();
}

void Interpreter::visit(Continue& s) {
    throw ContinueException();
}

void Interpreter::visit(Let& s) {
    Value value = s.expr ? getResult(s.expr) : nullptr;
    value.isMutable = s.isMutable;
    //cout << s.name << " mutability: " << value.isMutable << endl;
    env->define(s.name, value);
}

void Interpreter::visit(Return& s) {
    Value value = s.expr ? getResult(s.expr) : nullptr;
    throw ReturnException(value);
}

void Interpreter::visit(Class& s) {
    auto cls = std::make_shared<ClassObj>();

    cls->superclass = nullptr;

    for (auto& fieldMem : s.fieldMembers) {
        Value v = fieldMem.initialiser ? getResult(fieldMem.initialiser) : nullptr;
        v.isMutable = fieldMem.isMutable;
        cls->fieldDefaults[fieldMem.name] = v;
        //c.fields[fieldMem.name] = v;
    }

    for (auto& methodMem : s.methodMembers) {
        Value v = getResult(methodMem.methodExpr);
        FunctionPtr fn = std::static_pointer_cast<FunctionObj>(std::get<ObjectPtr>(v.data));
        cls->methods[methodMem.name] = fn;
    }

    env->define(s.name, Value(cls));
}

void Interpreter::visit(ExprStmt& s) {
    result = getResult(s.expr);
}
