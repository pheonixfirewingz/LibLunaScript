#pragma once
#include <stdint.h>
#include <string>
#include <vector>

typedef enum LexTokenEnum : uint8_t
{
    T_WHITESPACE,
    T_IDENTIFIER,
    T_FUNC,
    T_RET,
    T_R_SQUIGGLY,
    T_L_SQUIGGLY,
    T_R_CURLY,
    T_L_CURLY,
    T_COMMA,
    T_DOT,
    T_ADD,
    T_SUB,
    T_MUL,
    T_DIV,
    T_AND,
    T_EQUAL,
    T_OR,
    T_R_ARROW,
    T_L_ARROW,
    T_S_ARROW,
    T_MODULO,
    T_QUOTE,
    T_SINGLE_LETTER_QUOTE,
    //types
    T_INT8,
    T_INT16,
    T_INT32,
    T_INT64,
    T_UINT8,
    T_UINT16,
    T_UINT32,
    T_UINT64,
    T_FLOAT32,
    T_FLOAT64,
    T_STRING,
}LexTokenEnum;

typedef struct lexToken
{
    LexTokenEnum token;
    std::string str_token = "";
    uint32_t line = 0;
    explicit lexToken() = default;
    lexToken(LexTokenEnum in_tok, uint32_t int_in, std::string str_token) noexcept
        : token(in_tok)
        , str_token(str_token)
        , line(int_in)
    {
    }

    inline bool operator != (const lexToken tok) const
    {
        return token != tok.token && line != tok.line;
    }

    inline bool operator == (const lexToken tok) const
    {
        return token == tok.token && line == tok.line;
    }
} lexToken;

const std::vector<lexToken> lexIt(const std::string&& source) noexcept;