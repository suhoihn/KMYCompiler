#include "TypeInterner.hpp"

#include <string>
#include <unordered_map>
#include "../Core/type.hpp"
#include "../Core/AstBaseForward.hpp"

ArrayType* TypeInterner::getArrayType(Type* elementType) {
    auto it = arrayCache.find(elementType);
    if (it != arrayCache.end()) {
        return it->second;
    }

    // New element type for array.
    ArrayType* newArrType = new ArrayType(elementType);
    arrayCache[elementType] = newArrType;

    return newArrType;
}

FunctionType* TypeInterner::getFunctionType(
    std::vector<Type*> paramTypes, 
    Type* returnType
) {
    FunctionKey fnKey{paramTypes, returnType};

    auto it = fnCache.find(fnKey);
    if (it != fnCache.end()) {
        return it->second;
    }

    // New element type for function.
    FunctionType* newFnType = new FunctionType(paramTypes, returnType);
    fnCache[fnKey] = newFnType;

    return newFnType;
}

StructualType* TypeInterner::getStructualType(std::unordered_map<std::string, Type*> raw) {
    // IMPORTANT: {x:int, y:int} and {y:int, x:int} are considered the SAME types!
    RecordKey recordKey = makeKey(raw);

    auto it = recordCache.find(recordKey);
    if (it != recordCache.end()) {
        return it->second;
    }

    // New element type for record.
    StructualType* newRecordType = new StructualType();
    int offset = 0;
    for (auto& [name, type] : raw) {
        newRecordType->fieldTypes[name] = type;
        // TODO: This will be filled in later passes (namely closure analysis and slot allocation).
        newRecordType->layout[name] = INVALID_SLOT;
    }
    recordCache[recordKey] = newRecordType;

    return newRecordType;
}


RecordKey TypeInterner::makeKey(std::unordered_map<std::string, Type*> raw) {
    std::vector<FieldKey> fields;

    for (auto& [name, type] : raw) {
        fields.push_back({name, type});
    }

    std::sort(fields.begin(), fields.end(),
        [](const FieldKey& a, const FieldKey& b) {
            return a.name < b.name;
        });

    return {fields};
}
