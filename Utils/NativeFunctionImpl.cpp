#include "NativeFunctionImpl.hpp"

#include "variant"
#include "../Core/type.hpp"
#include "../Core/Value.hpp"

std::unordered_map<std::string, FunctionType*> nativeFnTypes = {
    {"isAlpha", new FunctionType({&Types::STRING_TYPE}, &Types::BOOL_TYPE)},
};

Value isAlpha(int argc, Value* args) {
    if (argc != 1) {
        throw std::runtime_error("isAlpha expects 1 argument");
    }

    if (!std::holds_alternative<std::string>(args[0].data)) {
        throw std::runtime_error("isAlpha expects string");
    }

    const std::string& s = std::get<std::string>(args[0].data);

    if (s.empty()) return false;

    return static_cast<bool>(
        std::isalpha(static_cast<unsigned char>(s[0]))
    );
}

Value subString(int argc, Value* args) {
    if (argc != 3) {
        throw std::runtime_error("isAlpha expects 2 arguments");
    }

    if (!std::holds_alternative<std::string>(args[0].data)) {
        throw std::runtime_error("isAlpha expects string");
    }

    const std::string& s = std::get<std::string>(args[0].data);

    if (s.empty()) return false;

    return static_cast<bool>(
        std::isalpha(static_cast<unsigned char>(s[0]))
    );
}
