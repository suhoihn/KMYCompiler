#pragma once
#include <string>

// Tokens are atomic units of meaning.
// We break those down as narrowly as possible to make parsing easier. 
enum class TokenType {
    // Literals
    Int,
    Double,
    Identifier,
    StringLiteral,

    // Arithmetic Operators
    Plus,
    Minus,
    Star,
    Slash,
    Percent,
    
    // Bitwise Operators
    BitAnd,
    BitOr,
    BitNot, // ~
    BitXor, // ^
    LShift, // <<
    RShift, // >>
    
    // Logical Operators
    LogicalAnd, 
    LogicalOr,
    
    // Assignment Operators
    Assign, // = (overloaded with declaration as well)
    PlusAssign,
    MinusAssign,
    StarAssign,
    SlashAssign,
    PercentAssign,
    BitAndAssign,
    BitOrAssign,
    BitXorAssign,
    LShiftAssign,
    RShiftAssign,
    LogicalAndAssign,
    LogicalOrAssign,
    
    // Relational Operators
    Greater,
    Less,
    GreaterEqual,
    LessEqual,
    EqualEqual,
    NotEqual,
    
    // Keywords
    KeywordPrint,
    KeywordIf,
    KeywordElse,
    KeywordWhile,
    KeywordFor,
    KeywordContinue,
    KeywordBreak,
    KeywordTrue,
    KeywordFalse,
    KeywordNull,
    KeywordLet,
    KeywordConst,// Old: KeywordMut,
    KeywordFun,
    KeywordReturn,
    KeywordThis,
    KeywordClass,
    KeywordNew,
    KeywordTypealias,
    KeywordRecord,
    KeywordInit,

    // Type keywords
    KeywordInt, // Not that this is different from just Int
    KeywordDouble, 
    KeywordBool,
    KeywordString,
    KeywordVoid,
    KeywordArray,  // ?
    KeywordObject, // ?
    KeywordAny,

    // Symbols
    LeftParen,
    RightParen,
    LeftBrace,
    RightBrace,
    Semicolon,
    Bang,
    LeftBracket,
    RightBracket,
    Comma,
    Dot,
    Colon,
    Ellipsis, // ...
    Arrow, // ->

    // Special
    EndOfFile,
    Unknown
};

// EXT 3.4: Token struct includes start and end indexes.
struct Token {
    TokenType type;
    std::string lexeme;
    int line;
    int column;
    int startIdx;
    int endIdx;
};