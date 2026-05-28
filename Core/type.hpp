#pragma once
#include <vector>
#include <memory>

enum class TypeNodeKind {
    NAMED,
    FUNCTION,
    ARRAY,
    RECORD
};

struct TypeNode {
    TypeNodeKind kind;
    TypeNode(TypeNodeKind kind) : kind(kind) {}
    virtual ~TypeNode() = default;
};

using TypeNodePtr = std::shared_ptr<TypeNode>;

struct NamedTypeNode : TypeNode {
    std::string name;

    NamedTypeNode(const std::string& name) 
        : TypeNode(TypeNodeKind::NAMED), name(name) {}
};

struct FunctionTypeNode : TypeNode {
    std::vector<TypeNodePtr> params;
    TypeNodePtr returnType;

    FunctionTypeNode(
        std::vector<TypeNodePtr> params_,
        TypeNodePtr returnType_
    )
        : TypeNode(TypeNodeKind::FUNCTION),
          params(std::move(params_)),
          returnType(std::move(returnType_))
    {}
};

struct RecordTypeNode : TypeNode {
    std::vector<std::pair<std::string, TypeNodePtr>> paramTypePairs;

    RecordTypeNode(
        std::vector<std::pair<std::string, TypeNodePtr>> paramTypePairs
    ) : TypeNode(TypeNodeKind::RECORD), paramTypePairs(std::move(paramTypePairs)) {}
};

struct ArrayTypeNode : TypeNode {
    TypeNodePtr elementType;
    int size;
    bool isSizeDetermined;
    bool isDynamic;

    ArrayTypeNode(TypeNodePtr elementType, int size, bool isSizeDetermined, bool isDynamic)
        : TypeNode(TypeNodeKind::ARRAY), elementType(std::move(elementType)), size(size), isSizeDetermined(isSizeDetermined), isDynamic(isDynamic) {}
};


enum class TypeKind {
    INT,
    DOUBLE,
    BOOL,
    STRING,
    NULLTYPE,
    ARRAY,
    AGGREGATE, // record or class definitions + annoymous records {...}
    INSTANCE, // class instances
    FUNCTION, // Just denotes that the type is a function. 
    VOID, // Denotes no type. Only used for functions.
    ANY,
    UNKNOWN,
    UNINITIALISED
};

struct Type {
    const TypeKind kind;

    Type(TypeKind kind) : kind(kind) {}
};

struct ParamTypeInfo {
    bool hasDefault;
    bool isVariadic;
    Type* type;
};


struct FunctionType : Type {
    std::vector<Type*> paramTypes;
    Type* returnType;

    std::vector<ParamTypeInfo> info;
    bool infoExists = false;

    // No parameter info exists.
    FunctionType(std::vector<Type*> paramTypes, Type* returnType) 
        : Type(TypeKind::FUNCTION), paramTypes(move(paramTypes)), returnType(returnType), infoExists(false) {}
 
    // Parameter info exists.
    FunctionType(std::vector<Type*> paramTypes, std::vector<ParamTypeInfo> info, Type* returnType) 
        : Type(TypeKind::FUNCTION), paramTypes(move(paramTypes)), info(move(info)), returnType(returnType), infoExists(true) {}
};

struct ArrayType : Type {
    Type* elementType;

    ArrayType(Type* elementType) : Type(TypeKind::ARRAY), elementType(elementType) {}
};

struct AggregateType : Type {
    std::unordered_map<std::string, Type*> fieldTypes;
    std::unordered_map<std::string, int> layout;

    AggregateType() : Type(TypeKind::AGGREGATE) {}
    AggregateType(std::unordered_map<std::string, Type*> fieldTypes, std::unordered_map<std::string, int> layout)
        : Type(TypeKind::AGGREGATE), fieldTypes(move(fieldTypes)), layout(move(layout)) {}
};

namespace Types {
    inline Type INT_TYPE = { TypeKind::INT };
    inline Type DOUBLE_TYPE = { TypeKind::DOUBLE };
    inline Type BOOL_TYPE = { TypeKind::BOOL };
    inline Type STRING_TYPE = { TypeKind::STRING };
    inline Type NULL_TYPE = { TypeKind::NULLTYPE };
    inline Type VOID_TYPE = { TypeKind::VOID };
    inline Type ANY_TYPE = { TypeKind::ANY };
    inline Type UNINITIALISED = { TypeKind::UNINITIALISED };
};