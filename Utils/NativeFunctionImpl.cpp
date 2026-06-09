#include "NativeFunctionImpl.hpp"

#include "variant"
#include "../Core/type.hpp"
#include "../Core/Value.hpp"
#include "../Semantics/TypeInterner.hpp"
#include <iostream>

Value isAlpha(int argc, Value* args) {
    std::cout << "Welcome to isAlpha: " << argc << std::endl;
    if (argc != 1) {
        throw std::runtime_error("isAlpha expects 1 argument");
    }

    if (!std::holds_alternative<std::string>(args[0].data)) {
        throw std::runtime_error("isAlpha expects string");
    }

    const std::string& s = std::get<std::string>(args[0].data);

    std::cout << "checking " << s << std::endl;
    if (s.empty()) return Value(false);

    std::cout << Value(static_cast<bool>(
        std::isalpha(static_cast<unsigned char>(s[0]))
    )).toString() << std::endl;
    
    return Value(static_cast<bool>(
        std::isalpha(static_cast<unsigned char>(s[0]))
    ));
}

Value subString(int argc, Value* args) {
    std::cout << "Welcome to substring: " << argc << std::endl;
    if (argc != 3) {
        throw std::runtime_error("subString expects 3 arguments");
    }

    if (!std::holds_alternative<std::string>(args[0].data)) {
        throw std::runtime_error("SEVERE (Compiler bug): First argument of subString expects string.");
    }
    if (!std::holds_alternative<int>(args[1].data)) {
        throw std::runtime_error("SEVERE (Compiler bug): Second argument of subString expects int.");
    }
    if (!std::holds_alternative<int>(args[2].data)) {
        throw std::runtime_error("SEVERE (Compiler bug): Third argument of subString expects int.");
    }

    const std::string& s = std::get<std::string>(args[0].data);
    std::cout << "checking " << s << std::endl;

    int startIdx = std::get<int>(args[1].data);
    int length = std::get<int>(args[2].data);

    return Value(s.substr(startIdx, length));
}

Value about(int argc, Value* args) {
    if (argc != 0) {
        throw std::runtime_error("about expects 0 arguments");
    }
    return Value(std::string("\
        KMY Language (c) 2026 KMY.\n\
        I just wanted to make this:\n\
        A language with Python style freedom with Typescript safety.\n\
        What? Are you arguing that those cannot coexist?\n\
        ...you are not wrong.\n\
        But you know what, I love learning low level language a lot. \n\
        Later, I don't know whether it would be in August, December, or even in 2027,\n\
        this would be eventually lowered in ASM x86-64.\n\
        That's my little dream. :)\n\
        Would that dream ever be achieved? Only future knows...\n\
        - Suho Ihn in June 6th.\n\
        "));
}

std::unordered_map<std::string, NativeEntry> nativeFnTypes = {
    {
        "isAlpha",
        {
            TypeInterner::getFunctionType(
                {&Types::STRING_TYPE},
                &Types::BOOL_TYPE
            ),
            &isAlpha,
            0
        }
    },
    {
        "subString",
        {
            TypeInterner::getFunctionType(
                {&Types::STRING_TYPE, &Types::INT_TYPE, &Types::INT_TYPE},
                &Types::STRING_TYPE
            ),
            &subString,
            1
        },
    },
    {
        "about",
        {
            TypeInterner::getFunctionType(
                std::vector<Type*>{},
                &Types::VOID_TYPE
            ),
            &about,
            2
        }
    }
};