#pragma once

#include "../Core/type.hpp"

struct SSAValue {
    int id;
    Type* type = nullptr;
};

inline std::ostream& operator<<(std::ostream& os, const SSAValue& value) {
    return os << "v" << value.id;
}