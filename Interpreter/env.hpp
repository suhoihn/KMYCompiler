#pragma once

#include <string>
#include <iostream>
#include <unordered_map>
#include "../Core/value.hpp"

class Env {
private:
    std::unordered_map<std::string, Value> values;
    Env *parent;
public:
    Env(Env *parent = nullptr);

    void define(const std::string& name, const Value& value);
    void assign(const std::string& name, const Value& value);
    Value get(const std::string& name) const;
    bool has(const std::string& name) const;

    void print() const;
};

struct EnvGuard {
    Env*& envRef;
    Env* previous;

    EnvGuard(Env*& currentEnv, Env* newEnv)
        : envRef(currentEnv), previous(currentEnv) {
        envRef = newEnv;
    }

    ~EnvGuard() {
        //std::cout << "Destoryed!\n";
        envRef = previous;
    }
};