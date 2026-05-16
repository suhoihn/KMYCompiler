#pragma once
#include <vector>
#include <memory>

enum class TypeNodeKind {
    NAMED,
    FUNCTION,
};

struct TypeNode {
    TypeNodeKind kind;
    TypeNode(TypeNodeKind kind) : kind(kind) {}
    virtual ~TypeNode() = default;
};

using TypeNodePtr = std::unique_ptr<TypeNode>;

struct NamedTypeNode : TypeNode {
    NamedTypeNode() : TypeNode(TypeNodeKind::NAMED) {}

    std::string name;
};

struct FunctionTypeNode : TypeNode {
    FunctionTypeNode() : TypeNode(TypeNodeKind::FUNCTION) {}

    std::vector<TypeNodePtr> params;
    TypeNodePtr returnType;
};

enum class TypeKind {
    INT,
    DOUBLE,
    BOOL,
    STRING,
    NULLTYPE,
    ARRAY,
    RECORD, // custom {...} or class instances.
    FUNCTION, // Just denotes that the type is a function. 
    VOID, // Denotes no type. Only for functions.
    ANY,
    UNKNOWN,
    UNINITIALISED
};

struct Type {
    const TypeKind kind;
};

struct FunctionType : Type {
    std::vector<Type*> paramTypes;
    const Type* const returnType;
};

struct ArrayType : Type {
    const Type* const elementType;
};

namespace Types {
    inline Type INT_TYPE = { TypeKind::INT };
    inline Type DOUBLE_TYPE = { TypeKind::DOUBLE };
    inline Type BOOL_TYPE = { TypeKind::BOOL };
    inline Type STRING_TYPE = { TypeKind::STRING };
    inline Type NULL_TYPE = { TypeKind::NULLTYPE };
    inline Type UNINITIALISED = { TypeKind::UNINITIALISED };
};