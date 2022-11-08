#pragma once
#include <string>
#include <vector>

typedef enum LexTokenEnum : uint8_t
{
    T_NONE,
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
    T_MODULO,
    T_QUOTE,
    T_SINGLE_LETTER_QUOTE,
    T_INTERNAL_ERROR
}LexTokenEnum;

typedef struct GmlSpan 
{
    size_t start;
    size_t end;

    explicit GmlSpan() = default;
    GmlSpan(size_t start_in, size_t end_in) noexcept
        : start(start_in)
        , end(end_in)
    {
    }
}GmlSpan;

typedef struct lexToken
{
    LexTokenEnum token = T_NONE;
    std::string extra_data = "";
    uint64_t line = 0;
    GmlSpan span = {0,0};
    explicit lexToken() = default;
    lexToken(LexTokenEnum in_tok, const char *str_in, uint64_t int_in, GmlSpan span_in) noexcept
        : token(in_tok)
        , extra_data(str_in)
        , line(int_in)
        , span(span_in)
    {
    }

    inline bool operator != (const uint16_t tok) const
    {
        return token != tok;
    }

    inline bool operator == (const uint16_t tok) const
    {
        return token == tok;
    }
} lexToken;

class Lexer
{
    std::vector<lexToken> tokens;
    std::string source;
    size_t index = 0;
    [[nodiscard]]inline bool eof() const noexcept { return index <= source.size(); };
    [[nodiscard]]char getNextChar() noexcept;
    [[nodiscard]]inline char peekNextChar()const noexcept;
    [[nodiscard]]GmlSpan parseSpanNextToken() noexcept;
    [[nodiscard]]bool _break(const char c) noexcept;
    public:
    explicit Lexer(std::string source_in);
    [[nodiscard]]inline bool eot() const noexcept { return index < tokens.size() - 1; };
    inline void skipNextToken() noexcept { index++; };
    [[nodiscard]] lexToken &getNextToken() noexcept
    {
        return tokens[std::min<size_t>(index++, tokens.size() - 1)];
    };
    [[nodiscard]] const lexToken &peekNextToken(uint64_t offset = 0) const noexcept
    {
        return tokens[std::min<size_t>(index + offset, tokens.size() - 1)];
    };
    [[nodiscard]]inline const std::string& refSource() const noexcept { return source;}; 
};