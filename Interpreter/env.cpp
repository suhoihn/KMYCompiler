#include "env.hpp"

#include <string>
#include <iostream>

Env::Env(Env* parent) : parent(parent) {}

void Env::define(const std::string& name, const Value& value) {
    // cout << "[DEBUG] Declaration of new variable: " << name << " = " << value.toString() << " Successful.\n";
    // IMPORTANT: overwrite can happen here. bare that in mind.
    values[name] = value;
}

void Env::assign(const std::string& name, const Value& value) {
    // Note that assign is not for making new variables.
    if (values.count(name)) {
        //Value prevValue = values[name];
        if(!values[name].isMutable) {
            throw std::runtime_error("This value (" + name + ") is not mutable.");
        }
        values[name].data = value.data;
        // cout << "[DEBUG] Assignment " << name << " = " << value.toString() << " Successful.\n";
        return;
    }
    
    if (parent != nullptr) {
        // cout << "[DEBUG] " << name << " not found in local scope. Moving to parent scope.\n";
        parent->assign(name, value);
        return;
    }

    // Handle not found error.
    throw std::runtime_error("Variable not found. You know u can improve this msg.");
}

Value Env::get(const std::string& name) const {
    if (values.count(name)) {
        // cout << "[DEBUG] " << name << " is " << values.at(name).toString() << "\n";
        
        return values.at(name);
    }
    
    if (parent != nullptr) {
        // cout << "[DEBUG] " << name << " not found in local scope. Moving to parent scope.\n";
        
        return parent->get(name);
    }

    // Handle not found error.
    throw std::runtime_error("Variable " + name + " not found. You know u can improve this msg.");
}

void Env::print() const {
    std::cout << "{ ";

    bool first = true;
    for (const auto& [key, value] : values) {
        if (!first) std::cout << ", ";
        std::cout << key << ": " << value.toString();
        first = false;
    }

    std::cout << " }\n";
}

bool Env::has(const std::string& name) const {
    if (values.count(name)) return true;
    if (parent) return parent->has(name);
    return false;
}