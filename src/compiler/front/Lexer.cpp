#include "Lexer.h"
#include "../../util/LookUpTable.h"
#include <regex>

static const internal_std::ReadOnlyLookupTable<size_t, LexTokenEnum> lookup = {
    {std::hash<std::string>{}("int8"), T_INT8},       {std::hash<std::string>{}("int16"), T_INT16},
    {std::hash<std::string>{}("int32"), T_INT32},     {std::hash<std::string>{}("int64"), T_INT64},
    {std::hash<std::string>{}("uint8"), T_UINT8},     {std::hash<std::string>{}("uint16"), T_UINT16},
    {std::hash<std::string>{}("uint32"), T_UINT32},   {std::hash<std::string>{}("uint64"), T_UINT64},
    {std::hash<std::string>{}("float32"), T_FLOAT32}, {std::hash<std::string>{}("float64"), T_FLOAT64},
    {std::hash<std::string>{}(" "), T_WHITESPACE},    {std::hash<std::string>{}("\t"), T_WHITESPACE},
    {std::hash<std::string>{}("func"), T_FUNC},       {std::hash<std::string>{}("ret"), T_RET},
    {std::hash<std::string>{}("{"), T_L_SQUIGGLY},    {std::hash<std::string>{}("}"), T_R_SQUIGGLY},
    {std::hash<std::string>{}("("), T_L_CURLY},       {std::hash<std::string>{}(")"), T_R_CURLY},
    {std::hash<std::string>{}(","), T_COMMA},         {std::hash<std::string>{}("."), T_DOT},
    {std::hash<std::string>{}("+"), T_ADD},           {std::hash<std::string>{}("-"), T_SUB},
    {std::hash<std::string>{}("/"), T_DIV},           {std::hash<std::string>{}("*"), T_MUL},
    {std::hash<std::string>{}("="), T_EQUAL},         {std::hash<std::string>{}("and"), T_AND},
    {std::hash<std::string>{}("or"), T_OR},           {std::hash<std::string>{}(">"), T_L_ARROW},
    {std::hash<std::string>{}("<"), T_R_ARROW},       {std::hash<std::string>{}("mod"), T_MODULO},
    {std::hash<std::string>{}("\""), T_QUOTE},        {std::hash<std::string>{}("\'"), T_SINGLE_LETTER_QUOTE},
    {std::hash<std::string>{}("xor"), T_XOR},
};

static const std::regex regex(
    "(([A-Za-z0-9-]+)|(\\s)|([\\\",\\\\',\\(,\\),\\[,\\],\\<,\\>,\\{,\\},\\,:,\\.])|([_,+,=,*,/]+))",
    std::regex_constants::icase);

static const internal_std::ReadOnlyLookupTable<size_t, bool> reject = {{std::hash<std::string>{}("\n"), true},
                                                                       {std::hash<std::string>{}("\r"), false}};

static inline const std::vector<std::string> tokenizeString(const std::string &&str)
{
    auto words_begin = std::sregex_token_iterator(str.begin(), str.end(), regex);
    auto words_end = std::sregex_token_iterator();
    std::vector<std::string> tokenized_strings;
    tokenized_strings.reserve(std::distance(words_begin, words_end));
    std::move(words_begin, words_end, std::back_inserter(tokenized_strings));
    return tokenized_strings;
}

const std::vector<lexToken> lexIt(const std::string &&source) noexcept
{
    uint32_t line = 0;
    std::vector<lexToken> tokens;
    auto split = tokenizeString(std::move(source));
    tokens.reserve(split.size());
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
            if (*o)
                line++;
            const uint32_t n = ++i;
            i = std::min<uint32_t>(size, n);
            continue;
        }
        lexToken token;
        auto t = lookup.find(std::hash<std::string>{}(word));
        if (t == NULL)
        {
            token.token = T_IDENTIFIER;
            token.str_token = std::move(word);
        }
        else
        {
            auto k = lookup.find(std::hash<std::string>{}(word));
            if (k == NULL)
                token.token = *t;
            else
            {
                if ((*k) == T_L_ARROW && (*t) == T_SUB)
                {
                    token.token = T_S_ARROW;
                    skip = true;
                }
                else
                    token.token = *t;
            }
        }
        token.line = line;
        const uint32_t n = ++i;
        i = std::min<uint32_t>(size, n);
        if (!(tokens.back() == token))
            tokens.emplace_back(token);
    }
    tokens.shrink_to_fit();
    return tokens;
}