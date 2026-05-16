#pragma once

#include <memory>
#include "../Core/value.hpp"

class RuntimeIterator {
public:
    virtual ~RuntimeIterator() = default;
    virtual bool hasNext() = 0;
    virtual Value next() = 0;
};

std::unique_ptr<RuntimeIterator> makeIter(const Value& value);
