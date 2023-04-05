#include "Lexer.h"
#include <iostream>
#include <cwctype>

namespace LunaScript::parser::lexer
{

#define CASE(X)                                               \
    case LunaScript::parser::lexer::TypedStringView::Type::X: \
        return L#X
static const wchar_t *toString(TypedStringView::Type type)
{
    switch (type)
    {
        CASE(IDENTIFIER);
        CASE(NUMBER_IDENTIFIER);
        CASE(IMPORT);
        CASE(MODULE);
        CASE(FUNC);
        CASE(PUBLIC_FUNC);
        CASE(RET);
        CASE(R_SQUIGGLY);
        CASE(L_SQUIGGLY);
        CASE(R_BOX);
        CASE(L_BOX);
        CASE(R_CURLY);
        CASE(L_CURLY);
        CASE(COMMA);
        CASE(DOT);
        CASE(HASH_TAG);
        CASE(ADD);
        CASE(ADD_EQUAL);
        CASE(ADD_ONE);
        CASE(SUB);
        CASE(SUB_EQUAL);
        CASE(SUB_ONE);
        CASE(MUL);
        CASE(MUL_EQUAL);
        CASE(DIV);
        CASE(DIV_EQUAL);
        CASE(AND);
        CASE(OR);
        CASE(XOR);
        CASE(MODULO);
        CASE(EQUAL);
        CASE(R_ARROW);
        CASE(L_ARROW);
        CASE(S_ARROW);
        CASE(GLOBAL);
        CASE(PTR);
        CASE(REF);
        CASE(INT8);
        CASE(INT16);
        CASE(INT32);
        CASE(INT64);
        CASE(UINT8);
        CASE(UINT16);
        CASE(UINT32);
        CASE(UINT64);
        CASE(FLOAT32);
        CASE(FLOAT64);
        CASE(STRING);
        CASE(NEW_LINE);
        CASE(EMOJI);
        CASE(TAB);
        CASE(SPACE);
    default:
        return L"ERROR";
    }
}

void TypedStringView::print(std::wstring)
{
    std::wcout << L"(TOKEN: {TYPE: " << toString(type) << L", STRING: \"";
    std::wcout << span << L"\", LINE: " << line << L"})\n";
}

static constexpr struct Combiners
{
    const wchar_t one, two;
} list[] = {{L'-', L'>'}, {L'+', L'+'}, {L'-', L'-'}, {L'+', L'='}, {L'=', L'+'}, {L'-', L'='}, {L'=', L'-'},
            {L'*', L'='}, {L'=', L'*'}, {L'/', L'='}, {L'=', L'/'}, {L'/', L'*'}, {L'*', L'/'}};

static inline bool canCombine(const std::vector<std::wstring_view> &result)
{
    for (const Combiners &t : list)
        if (result.size() >= 2 && result[result.size() - 2][0] == t.one && result.back()[0] == t.two)
            return true;
    return false;
}

static const inline std::vector<std::wstring_view> split(const std::wstring &str, const wchar_t *delimiter)
{
    std::vector<std::wstring_view> result;
    size_t start = 0;
    size_t end = str.find_first_of(delimiter);
    while (end != std::wstring::npos)
    {
        std::wstring_view view(str.data() + start, end - start);
        if (!view.empty())
            result.push_back(view);
        result.push_back(std::wstring_view(str.data() + end, 1));
        start = end + 1;
        end = str.find_first_of(delimiter, start);
        // Check if the previous and current views form a special case
        if (result.back().size() == 1)
        {
            if (canCombine(result))
            {
                result.pop_back();
                std::wstring_view two = result.back();
                result.pop_back();
                std::wstring_view new_view(two.data(), 2);
                result.push_back(new_view);
            }
            if (result.size() >= 4 && result[result.size() - 4][0] == L' ' && result[result.size() - 3][0] == L' ' &&
                result[result.size() - 2][0] == L' ' && result.back()[0] == L' ')
            {
                result.pop_back();
                result.pop_back();
                result.pop_back();
                std::wstring_view two = result.back();
                result.pop_back();
                std::wstring_view new_view(two.data(), 4);
                result.push_back(new_view);
            }
        }
    }
    std::wstring_view last(str.data() + start, str.length() - start);
    if (!last.empty())
        result.push_back(last);
    return result;
}

static inline bool is_number(const std::wstring_view &str) noexcept
{
    if (str.empty())
        return false;
    else if (!std::iswdigit(str[0]))
        return false;
    std::wstring test(str.begin(), str.end());
    wchar_t *end_ptr = nullptr;
    (void)std::wcstoul(test.c_str(), &end_ptr, 10);

    return (*end_ptr == L'\0');
}

static const inline TypedStringView::Type determineType(const std::wstring_view &src)
{
    if (is_number(src))
        return TypedStringView::Type::NUMBER_IDENTIFIER;

    switch (src.size())
    {
    case 1: {
        switch (src[0])
        {
        case L'.':
            return TypedStringView::Type::DOT;
        case L',':
            return TypedStringView::Type::COMMA;
        case L'#':
            return TypedStringView::Type::HASH_TAG;
        case L'+':
            return TypedStringView::Type::ADD;
        case L'-':
            return TypedStringView::Type::SUB;
        case L'/':
            return TypedStringView::Type::DIV;
        case L'*':
            return TypedStringView::Type::MUL;
        case L'=':
            return TypedStringView::Type::EQUAL;
        case L' ':
            return TypedStringView::Type::SPACE;
        case L'\t':
            return TypedStringView::Type::TAB;
        case L'\n':
            return TypedStringView::Type::NEW_LINE;
        case L'(':
            return TypedStringView::Type::L_CURLY;
        case L')':
            return TypedStringView::Type::R_CURLY;
        case L'{':
            return TypedStringView::Type::L_SQUIGGLY;
        case L'}':
            return TypedStringView::Type::R_SQUIGGLY;
        case L'<':
            return TypedStringView::Type::L_ARROW;
        case L'>':
            return TypedStringView::Type::R_ARROW;
        case L'[':
            return TypedStringView::Type::L_BOX;
        case L']':
            return TypedStringView::Type::R_BOX;
        default:
            return TypedStringView::Type::IDENTIFIER;
        }
    }
    break;
    case 2: {
        const wchar_t str[2] = {src[0], src[1]};
        if (wcscmp(str, L"/*") || wcscmp(str, L"*/"))
            return TypedStringView::Type::MULTI_COMMENT;
        if (wcscmp(str, L"=+") || wcscmp(str, L"+="))
            return TypedStringView::Type::ADD_EQUAL;
        else if (wcscmp(str, L"=-") || wcscmp(str, L"-="))
            return TypedStringView::Type::SUB_EQUAL;
        else if (wcscmp(str, L"=/") || wcscmp(str, L"/="))
            return TypedStringView::Type::SUB_EQUAL;
        else if (wcscmp(str, L"++"))
            return TypedStringView::Type::ADD_ONE;
        else if (wcscmp(str, L"--"))
            return TypedStringView::Type::SUB_ONE;
        else if (wcscmp(str, L"or"))
            return TypedStringView::Type::OR;
        else
            return TypedStringView::Type::IDENTIFIER;
    }
    break;
    case 3: {
        const wchar_t str[3] = {src[0], src[1], src[2]};
        if (wcscmp(str, L"ret"))
            return TypedStringView::Type::RET;
        else if (wcscmp(str, L"mod"))
            return TypedStringView::Type::MODULO;
        else if (wcscmp(str, L"and"))
            return TypedStringView::Type::AND;
        else if (wcscmp(str, L"xor"))
            return TypedStringView::Type::XOR;
        else if (wcscmp(str, L"ptr"))
            return TypedStringView::Type::PTR;
        else if (wcscmp(str, L"ref"))
            return TypedStringView::Type::REF;
        else
            return TypedStringView::Type::IDENTIFIER;
    }
    break;
    case 4: {
        const wchar_t str[4] = {src[0], src[1], src[2], src[3]};
        if (wcscmp(str, L"    "))
            return TypedStringView::Type::TAB;
        else if (wcscmp(str, L"func"))
            return TypedStringView::Type::FUNC;
        else if (wcscmp(str, L"int8"))
            return TypedStringView::Type::FUNC;
        else
            return TypedStringView::Type::IDENTIFIER;
    }
    break;
    case 5: {
        const wchar_t str[5] = {src[0], src[1], src[2], src[3], src[4]};
        if (wcscmp(str, L"uint8"))
            return TypedStringView::Type::UINT8;
        else if (wcscmp(str, L"int16"))
            return TypedStringView::Type::INT16;
        else if (wcscmp(str, L"int32"))
            return TypedStringView::Type::INT32;
        else if (wcscmp(str, L"int64"))
            return TypedStringView::Type::INT64;
        else
            return TypedStringView::Type::IDENTIFIER;
    }
    break;
    case 6: {
        const wchar_t str[6] = {src[0], src[1], src[2], src[3], src[4], src[5]};
        if (wcscmp(str, L"string"))
            return TypedStringView::Type::STRING;
        else if (wcscmp(str, L"public"))
            return TypedStringView::Type::PUBLIC_FUNC;
        else if (wcscmp(str, L"global"))
            return TypedStringView::Type::GLOBAL;
        else if (wcscmp(str, L"module"))
            return TypedStringView::Type::MODULE;
        else if (wcscmp(str, L"import"))
            return TypedStringView::Type::IMPORT;
        else if (wcscmp(str, L"uint16"))
            return TypedStringView::Type::UINT16;
        else if (wcscmp(str, L"uint32"))
            return TypedStringView::Type::UINT32;
        else if (wcscmp(str, L"uint64"))
            return TypedStringView::Type::UINT64;
        else
            return TypedStringView::Type::IDENTIFIER;
    }
    break;
    default:
        return TypedStringView::Type::IDENTIFIER;
    }
}

std::vector<TypedStringView> Lexer::operator()(const std::wstring &src) const
{

    std::vector<TypedStringView> tokens;
    const std::vector<std::wstring_view> splits = split(src, L"\n\"\'(){}[]-=+/*<>.,\\| \t;#");
    size_t line = 0;
    for (std::wstring_view split : splits)
    {
        TypedStringView view(determineType(split), split, line);
        tokens.push_back(view);
        if (split.compare(L"\n"))
            line++;
    }
    return tokens;
}
} // namespace LunaScript::parser::lexer