#pragma once
#include <libos/DataType.h>
#include <string>
#include <vector>

namespace LunaScript::parser::lexer
{
struct TypedStringView
{
    const enum class Type : uint8_t
    {
        IDENTIFIER, // A-Z + _
        NUMBER_IDENTIFIER, // 0-9
        IMPORT, // import
        MODULE, // module
        FUNC, // func
        PUBLIC_FUNC, // public
        RET,                    // ret
        R_SQUIGGLY, // }
        L_SQUIGGLY, // {
        R_BOX,      // ]
        L_BOX,      // [
        R_CURLY,    // )
        L_CURLY,    //(
        COMMA,    // ,
        DOT,  // .
        HASH_TAG,      // #
        MULTI_COMMENT, // /* */
        ADD, // +
        ADD_EQUAL, // += or =+
        ADD_ONE, // ++
        SUB, // - 
        SUB_EQUAL, // -= or =-
        SUB_ONE, // --
        MUL, // * 
        MUL_EQUAL, // *= or =*
        DIV, // /
        DIV_EQUAL, // /= or =/
        AND, // and
        OR, // or
        XOR, // xor
        MODULO, // mod
        EQUAL, // =
        R_ARROW, // >
        L_ARROW, // <
        S_ARROW, // ->
        GLOBAL, // global
        PTR, //ptr
        REF, //ref
        INT8, // int8
        INT16, // int16
        INT32, // int32
        INT64, // int64
        UINT8, // uint8
        UINT16, // uint16
        UINT32, // uint32
        UINT64, // uint64
        FLOAT32, // float32
        FLOAT64, // float64
        STRING, // string
        NEW_LINE, // \n
        EMOJI, // special other
        TAB, // \t or "    "
        SPACE // " "
    } type;
    const std::wstring_view span;
    const size_t line = 0;
    TypedStringView(const Type type_in, const std::wstring_view span_in, const size_t line_in)
        : type(type_in)
        , span(span_in)
        , line(line_in)
    {
    }

    void print(std::wstring src);
};

struct Lexer
{
    Lexer() = default;
    ~Lexer() = default;
    std::vector<TypedStringView> operator()(const std::wstring &input) const;
};

} // namespace LunaScript::parser::lexer