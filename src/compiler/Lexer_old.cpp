#include "Lexer.h"
#include "../util/LookUpTable.h"
std::string x();

char Lexer::getNextChar() noexcept
{
    return source[std::min<size_t>(index++, source.size())];
}

char Lexer::peekNextChar() const noexcept
{
    return source[std::min<size_t>(index, source.size())];
}

bool Lexer::_break(const char c) noexcept
{
    if (!eof())
        return true;
    switch (c)
    {
    case '\0':
    case '+':
    case '&':
    case '%':
    case '/':
    case '*':
    case '-':
    case '=':
    case ' ':
    case '\n':
    case '(':
    case ')':
    case '{':
    case '}':
    case '[':
    case ']':
    case ':':
    case ';':
    case ',':
    case '.':
    case '\"':
    case '\'':
        return true;
    default:
        return false;
    }
}

GmlSpan Lexer::parseSpanNextToken() noexcept
{
    uint64_t start = index;
    while (eof())
    {
        if (_break(getNextChar()))
            break;
        if (_break(peekNextChar()))
            break;
    }
    uint64_t offset = index;
    return {start, offset};
}

const size_t reject_lookup[] = {
    std::hash<std::string>{}("\n"), std::hash<std::string>{}(" "),
    std::hash<std::string>{}("\r"), std::hash<std::string>{}("\t"),
    std::hash<std::string>{}(";")};

const internal_std::ReadOnlyLookupTable<size_t, LexTokenEnum> lookup = {
    {std::hash<std::string>{}("func"), T_FUNC},    {std::hash<std::string>{}("ret"), T_RET},
    {std::hash<std::string>{}("{"), T_L_SQUIGGLY}, {std::hash<std::string>{}("}"), T_R_SQUIGGLY},
    {std::hash<std::string>{}("("), T_L_CURLY},    {std::hash<std::string>{}(")"), T_R_CURLY},
    {std::hash<std::string>{}(","), T_COMMA},      {std::hash<std::string>{}("."), T_DOT},
    {std::hash<std::string>{}("+"), T_ADD},        {std::hash<std::string>{}("-"), T_SUB},
    {std::hash<std::string>{}("/"), T_DIV},        {std::hash<std::string>{}("*"), T_MUL},
    {std::hash<std::string>{}("="), T_EQUAL},      {std::hash<std::string>{}("and"), T_AND},
    {std::hash<std::string>{}("or"), T_OR},        {std::hash<std::string>{}(">"), T_L_ARROW},
    {std::hash<std::string>{}("<"), T_R_ARROW},    {std::hash<std::string>{}("%"), T_MODULO},
    {std::hash<std::string>{}("\""), T_QUOTE},     {std::hash<std::string>{}("\'"), T_SINGLE_LETTER_QUOTE},
};

Lexer::Lexer(std::string source_in)
{
    source = std::move(source_in);
    uint64_t line = 0;
    while (eof())
    {
        GmlSpan span = parseSpanNextToken();
        std::string word = source.substr(span.start, std::max<size_t>(1, span.end - span.start));
        if (word == "\0")
            continue;
        size_t view = std::hash<std::string>{}(word);
        bool found = false;
        for (auto &item : reject_lookup)
        {
            if (item == view)
            {
                found = true;
                break;
            }
        }
        if (view == reject_lookup[0])
            line++;
        if (found)
            continue;
        lexToken token;
        token.line = line;
        token.span = std::move(span);

        for (auto &item : lookup)
        {
            if (item == view)
            {
                token.token = item.value;
                found = true;
                break;
            }
        }
        if (!found)
        {
            token.token = T_IDENTIFIER;
            token.extra_data = source.substr(span.start, std::max<size_t>(1, span.end - span.start));
        }
        tokens.emplace_back(std::move(token));
    }
    tokens.emplace_back(lexToken{T_INTERNAL_ERROR, "", 0, {0, 0}});
    index = 0;
}