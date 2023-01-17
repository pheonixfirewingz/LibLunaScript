#pragma once
#include <extend_std/Vector.h>
#include <regex>
#include <string>
#include <vector>
namespace LunaScript::assembler
{
enum class LexerToken
{
    IDENTIFIER,
    LABEL,
    R1,
    R2,
    R3,
    R4,
    R5,
    R6,
    R7,
    ADD,
    SUB,
    DIV,
    MUL,
    JMP,
    SKIP,
    CMP,
    NCMP,
    PUSH,
    POP,
    STORE,
    LOAD,
    CALL,
    ICALL,
    RET,
    MOV,
    NEW_LINE,
    CONST,
};

struct LexToken
{
    LexerToken token;
    std::string str_token = "";
    explicit LexToken() = default;
    LexToken(LexerToken in_tok, std::string str_token) noexcept
        : token(in_tok)
        , str_token(str_token)
    {
    }
};

class Lexer
{
    std::vector<LexToken> tokens;

  public:
    explicit Lexer(std::string) noexcept;

    const std::ReadOnlyVector<LexToken> retrieve() noexcept
    {
        return std::ReadOnlyVector<LexToken>::lock(tokens);
    }
};
} // namespace LunaScript::assembler