#pragma once
#include <util/ComplierString.hpp>
#include <vector>

namespace LunaScript::parser::lexer
{
struct LexicalTypedSpan
{
    const enum class Type : uint8_t
    {
        IMPORT,
        MODULE_NAME,
        IDENTIFIER,
        FUNC,
        PUBLIC_FUNC,
        RET,
        R_SQUIGGLY,
        L_SQUIGGLY,
        R_CURLY,
        L_CURLY,
        COMMA,
        DOT,
        ADD,
        SUB,
        MUL,
        DIV,
        AND,
        OR,
        XOR,
        MODULO,
        EQUAL,
        R_ARROW,
        L_ARROW,
        S_ARROW,
        NO_RETURN,
        GLOBAL,
        INT8,
        INT16,
        INT32,
        INT64,
        UINT8,
        UINT16,
        UINT32,
        UINT64,
        FLOAT32,
        FLOAT64,
        STRING,
        LETTER,
        NEW_LINE,
        TAB,
        SPACE
    } type;
    const stl::unicodespan span;
    LexicalTypedSpan(const Type type_in, const stl::unicodespan span_in)
        : type(type_in)
        , span(span_in)
    {
    }

    void print(stl::unicodestring src);
};

std::vector<LexicalTypedSpan> tokenize(const stl::unicodestring &source);
} // namespace LunaScript::parser::lexer