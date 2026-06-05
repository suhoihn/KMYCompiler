#pragma once

#include <algorithm>
#include <string>
#include <unordered_map>
#include "../Core/type.hpp"
#include "../Core/AstBaseForward.hpp"

// Used for the unordered_map for function types.
// TODO: Reuse ParamInfo and ParamKey???
struct FunctionKey {
    std::vector<Type*> params;
    Type* ret;

    bool operator==(const FunctionKey& other) const {
        return params == other.params && ret == other.ret;
    }
};

struct FunctionHash {
    size_t operator()(const FunctionKey& k) const {
        size_t h = 0;
        for (auto p : k.params)
            h ^= std::hash<void*>()(p) + 0x9e3779b9 + (h << 6) + (h >> 2);
        h ^= std::hash<void*>()(k.ret);
        return h;
    }
};

// TODO: Just use FieldInfo?
// TODO that doesnt belong here lol: Symbols only contain name, isMutable, type, and others are purely for debug.
struct FieldKey {
    std::string name;
    Type* type;

    bool operator==(const FieldKey& other) const {
        return name == other.name && type == other.type;
    }
};

struct RecordKey {
    std::vector<FieldKey> fields;
    
    bool operator==(const RecordKey& other) const {
        return fields == other.fields;
    }
};

struct RecordKeyHash {
    size_t operator()(const RecordKey& k) const {
        size_t seed = 0;

        for (auto& f : k.fields) {
            size_t h1 = std::hash<std::string>()(f.name);
            size_t h2 = std::hash<Type*>()(f.type);

            size_t combined = h1 ^ (h2 << 1);

            seed ^= combined + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        }

        return seed;
    }
};

class TypeInterner {
private:
    std::unordered_map<Type*, ArrayType*> arrayCache;
    std::unordered_map<FunctionKey, FunctionType*, FunctionHash> fnCache;
    std::unordered_map<RecordKey, StructualType*, RecordKeyHash> recordCache;
    RecordKey makeKey(std::unordered_map<std::string, Type*> raw);
public:
    ArrayType* getArrayType(Type* elementType);
    FunctionType* getFunctionType(std::vector<Type*> paramTypes, Type* returnType);
    StructualType* getStructualType(std::unordered_map<std::string, Type*> raw);
};
