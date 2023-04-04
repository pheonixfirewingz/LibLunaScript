#pragma once
#include <libos/DataType.h>
#include <vector>

namespace LunaScript::parser::lexer
{
struct TypedStringView
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
        EMOJI,
        TAB,
        SPACE
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
  private:
    static const std::vector<std::wstring_view> split(const std::wstring &str, const wchar_t delimiter);
    static constexpr inline bool isUnicode(const wchar_t &c) noexcept;
  public:
    Lexer() = default;
    ~Lexer() = default;
    std::vector<TypedStringView> operator()(const std::wstring &input) const;
};

} // namespace LunaScript::parser::lexer