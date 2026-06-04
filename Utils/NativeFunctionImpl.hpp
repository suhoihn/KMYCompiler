#pragma once

#include <string>
#include "../Core/type.hpp"
#include "../Core/value.hpp"

std::unordered_map<std::string, FunctionType*> nativeFnTypes;

Value isAlpha(int argc, Value* args);