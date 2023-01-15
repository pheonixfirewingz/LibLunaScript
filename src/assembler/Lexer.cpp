#include "Lexer.h"
#include <extend_std/LookUpTable.h>
#include <libos/Defines.h>

namespace LunaScript::assembler
{
static const std::regex regex("(([A-Za-z0-9-_]+)|(\\s)|(:))", std::regex_constants::icase);

static const std::ReadOnlyLookupTable<data_size_t, LexerToken> lookup = {
    {std::hash<std::string>{}(":"), LexerToken::LABEL},     {std::hash<std::string>{}("r1"), LexerToken::R1},
    {std::hash<std::string>{}("r2"), LexerToken::R2},       {std::hash<std::string>{}("r3"), LexerToken::R3},
    {std::hash<std::string>{}("r4"), LexerToken::R4},       {std::hash<std::string>{}("r5"), LexerToken::R5},
    {std::hash<std::string>{}("r6"), LexerToken::R6},       {std::hash<std::string>{}("r7"), LexerToken::R7},
    {std::hash<std::string>{}("add"), LexerToken::ADD},     {std::hash<std::string>{}("sub"), LexerToken::SUB},
    {std::hash<std::string>{}("div"), LexerToken::DIV},     {std::hash<std::string>{}("mul"), LexerToken::MUL},
    {std::hash<std::string>{}("jmp"), LexerToken::JMP},     {std::hash<std::string>{}("cmp"), LexerToken::CMP},
    {std::hash<std::string>{}("ncmp"), LexerToken::NCMP},   {std::hash<std::string>{}("push"), LexerToken::PUSH},
    {std::hash<std::string>{}("pop"), LexerToken::POP},     {std::hash<std::string>{}("store"), LexerToken::STORE},
    {std::hash<std::string>{}("load"), LexerToken::LOAD},   {std::hash<std::string>{}("call"), LexerToken::CALL},
    {std::hash<std::string>{}("icall"), LexerToken::ICALL}, {std::hash<std::string>{}("ret"), LexerToken::RET},
    {std::hash<std::string>{}("const"), LexerToken::CONST}, {std::hash<std::string>{}("mov"), LexerToken::MOV},
    {std::hash<std::string>{}("fadd"), LexerToken::FADD},   {std::hash<std::string>{}("fsub"), LexerToken::FSUB},
    {std::hash<std::string>{}("fdiv"), LexerToken::FDIV},   {std::hash<std::string>{}("fmul"), LexerToken::FMUL},
    {std::hash<std::string>{}("\n"), LexerToken::NEW_LINE},
};

static const std::ReadOnlyLookupTable<data_size_t, bool> reject = {
    {std::hash<std::string>{}("\r"), false},
    {std::hash<std::string>{}(" "), false},
    {std::hash<std::string>{}("\t"), false},
};

Lexer::Lexer(std::string source) noexcept
{
    auto words_begin = std::sregex_token_iterator(source.begin(), source.end(), regex);
    auto words_end = std::sregex_token_iterator();
    std::vector<std::string> split;
    split.reserve(std::distance(words_begin, words_end));
    std::move(words_begin, words_end, std::back_inserter(split));
    tokens.reserve(split.size() + 1);
    uint32_t i = 1;
    const uint32_t size = split.size() - 1;
    bool skip = false;
    for (const auto &word : split)
    {
        if (skip)
        {
            skip = false;
            const uint32_t n = ++i;
            i = std::min<uint32_t>(size, n);
            continue;
        }
        auto o = reject.find(std::hash<std::string>{}(word));
        if (o != NULL)
        {
            const uint32_t n = ++i;
            i = std::min<uint32_t>(size, n);
            continue;
        }
        LexToken token;
        auto t = lookup.find(std::hash<std::string>{}(word));
        if (t == NULL)
        {
            token.token = LexerToken::IDENTIFIER;
            token.str_token = std::move(word);
        }
        else
            token.token = *t;
        const uint32_t n = ++i;
        i = std::min<uint32_t>(size, n);
        if (tokens.size() < i)
            tokens.emplace_back(token);
    }
    if (tokens[tokens.size() - 1].token != LexerToken::NEW_LINE)
        tokens.push_back({LexerToken::NEW_LINE, ""});
    tokens.shrink_to_fit();
}
} // namespace LunaScript::assembler