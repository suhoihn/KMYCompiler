#pragma once
#include <string>
#include <exception>

// TODO: allow line column, start, end
class KMYParseError : public std::exception {

public:
    KMYParseError(const std::string& message) : msg(message) {}
    const char* what() const noexcept override {
        return msg.c_str();
    }

private:
    std::string msg;
};

class KMYLexError : public std::exception {

public:
    KMYLexError(const std::string& message) : msg(message) {}
    const char* what() const noexcept override {
        return msg.c_str();
    }

private:
    std::string msg;
};


class KMYCompileError : public std::exception {

public:
    KMYCompileError(const std::string& message) : msg(message) {}
    const char* what() const noexcept override {
        return msg.c_str();
    }

private:
    std::string msg;
};