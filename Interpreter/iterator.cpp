#include "iterator.hpp"

#include <stdexcept>

class RuntimeIterator {
public:
    virtual ~RuntimeIterator() = default;
    virtual bool hasNext() = 0;
    virtual Value next() = 0;
};

class StringIterator : public RuntimeIterator {
    std::string::const_iterator current, end;

public:
    StringIterator(const std::string& str)
        : current(str.begin()), end(str.end()) {}

    bool hasNext() override {
        return current != end;
    }

    Value next() override {
        return std::string(1, *current++);
    }
};

class ArrayIterator : public RuntimeIterator {
    std::vector<Value>::const_iterator current, end;

public:
    ArrayIterator(const std::vector<Value>& arr)
        : current(arr.begin()), end(arr.end()) {}

    bool hasNext() override {
        return current != end;
    }

    Value next() override {
        return *current++;
    }
};

std::unique_ptr<RuntimeIterator> makeIterator(const Value& value) {
    if (auto str = std::get_if<std::string>(&value.data))
        return std::make_unique<StringIterator>(*str);

    if (auto arr = std::get_if<Array>(&value.data))
        return std::make_unique<ArrayIterator>(*arr);

    throw std::runtime_error("Value is not iterable");
}