#pragma once
#include <variant>
#include <string>
#include <vector>
#include <memory>
#include <functional>
#include "AstBaseForward.hpp"
#include "../Core/operators.hpp"

// Pointers should be used to avoid circular dependencies...
// Unused
enum class ValueType_n {
    Int,
    Bool,
    Double,
    Null,
    String,
    Object // Heap allocated stuffs
};

// Unused
struct Value_n {
    ValueType_n type;
    
    union {
        int i;
        bool b;
        double d;
        void* obj;
    } data;
};


enum class ObjKind {
    Array,
    Function,
    NativeFn,
    BoundFn,
    Record,
    Class
};

// This represents HEAP ALLOCATED VALUES (not {}; they are records.)
struct Object {
    ObjKind kind;
    Object(ObjKind kind) : kind(kind) {}
    virtual ~Object() = default;
};

using ObjectPtr = std::shared_ptr<Object>;

using ValueType = std::variant<
    std::nullptr_t,
    int,
    double,
    bool,
    std::string,
    ObjectPtr // Heap allocated values
>;

// No runtime heap-allocated value. (For compiler's constantMap)
// Bascially, PRIMITIVES.
using ConstValue = std::variant<
    std::nullptr_t,
    int,
    double,
    bool,
    std::string
>;

struct Value {
    ValueType data;
    bool isMutable=true; // Default is mutable.

    Value() : data(nullptr) {}
    Value(std::nullptr_t) : data(nullptr) {}
    Value(int i) : data(i) {}
    Value(double v) : data(v) {}
    Value(bool b) : data(b) {}
    Value(const std::string& s) : data(s) {}
    Value(const ObjectPtr& o) : data(o) {}

    std::string toString() const;
};

using ArrayPtr = std::shared_ptr<struct ArrayObj>;
using FunctionPtr = std::shared_ptr<struct FunctionObj>;
using ClassPtr = std::shared_ptr<struct ClassObj>;

struct ArrayObj : Object {
    std::vector<Value> array;

    ArrayObj() : Object(ObjKind::Array) {}

    ArrayObj(const std::vector<Value>& array): Object(ObjKind::Array), array(array) {}
};

// A shared mutable cell for closures
// Runtime object.
struct Upvalue {
    Value* location;
    Value closed; // The value in stack is copied.
    bool isClosed = false;
};

using UpvaluePtr = std::shared_ptr<Upvalue>;

struct FunctionObj : Object {
    std::vector<Parameter> params;
    StmtPtr body; // For interpreter.
    Chunk chunk; // For compiler.
    
    // For closures
    std::vector<UpvaluePtr> upvalues; 

    FunctionObj (
        const std::vector<Parameter>& params,
        StmtPtr body
    ) : Object(ObjKind::Function), params(move(params)), body(move(body)) {}
};

struct NativeFnObj : Object {
    std::function<Value(const std::vector<Value>&)> func;

    NativeFnObj(std::function<Value(const std::vector<Value>&)> f)
        : Object(ObjKind::NativeFn),
          func(std::move(f)) {}
};

struct RecordObj : Object {
    std::unordered_map<std::string, Value> fields;
    ClassPtr cls = nullptr;

    RecordObj() : Object(ObjKind::Record) {}

    RecordObj(
        std::unordered_map<std::string, Value> fields,
        ClassPtr cls = nullptr
    ) : Object(ObjKind::Record), fields(std::move(fields)), cls(std::move(cls)) {}
};

struct BoundFnObj : Object {
    FunctionPtr fn;
    ObjectPtr receiver; // The object the function is called from.
    // i.e., "receiver" called "fn"

    BoundFnObj(FunctionPtr fn, ObjectPtr receiver)
        : Object(ObjKind::BoundFn),
        fn(std::move(fn)),
        receiver(std::move(receiver)) {}
};

struct ClassObj : Object {
    // Used for instantiating objects.
    // When a new instance is created, all values in fieldDefaults are COPIED.
    // IMPORTANT: This does not copy functions. Only fields.
    
    // unordered_map<string, Value> fields; // TODO: static fields. 

    std::unordered_map<std::string, Value> fieldDefaults;
    std::unordered_map<std::string, FunctionPtr> methods;
    ClassPtr superclass = nullptr;

    ClassObj() : Object(ObjKind::Class) {}

    ClassObj(
        std::unordered_map<std::string, Value> fieldDefaults,
        std::unordered_map<std::string, FunctionPtr> methods,
        ClassPtr superclass
    ) :
        Object(ObjKind::Class),
        fieldDefaults(std::move(fieldDefaults)),
        methods(std::move(methods)),  
        superclass(std::move(superclass)) {}
};


inline Value constValToVal(const ConstValue& v) {
    return std::visit([](auto&& arg) -> Value {
        return Value(arg);
    }, v);
}


Value applyBinary(BinaryOp op, const Value& left, const Value& right);

Value applyUnary(UnaryOp op, const Value& v);

bool isTruthy(const Value& v);
bool isEqual(const Value& a, const Value& b);
ObjKind typeToObjectKind(TypeKind t);
bool checkObjType(const Value& obj, ObjKind k);
bool typeMatches(const Value& v, const Type& t);
bool isNumber(const Value& v);
bool isDouble(const Value& v);
bool isInt(const Value& v);

ArrayPtr getArray(const Value& v);