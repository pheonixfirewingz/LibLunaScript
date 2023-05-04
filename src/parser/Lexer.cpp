#include "Lexer.h"
#include <cwctype>
namespace LunaScript::parser::lexer
{

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

static const inline std::vector<std::wstring_view> split(const std::wstring &src)
{
    std::vector<std::wstring_view> result;
    size_t start = 0;
    size_t end = src.find_first_of(L"\n\"\'(){}[]-=+/*<>.,\\| \t;#");
    while (end != std::wstring::npos)
    {
        std::wstring_view view(src.data() + start, end - start);
        if (!view.empty())
            result.push_back(view);
        result.push_back(std::wstring_view(src.data() + end, 1));
        start = end + 1;
        end = src.find_first_of(L"\n\"\'(){}[]-=+/*<>.,\\| \t;#", start);
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
    std::wstring_view last(src.data() + start, src.length() - start);
    if (!last.empty())
        result.push_back(last);
    return result;
}

static inline bool is_number(const std::wstring_view &src) noexcept
{
    if (src.empty())
        return false;
    else if (!std::iswdigit(src[0]))
        return false;
    std::wstring test(src.begin(), src.end());
    wchar_t *end_ptr = nullptr;
    (void)std::wcstoul(test.c_str(), &end_ptr, 10);

    return (*end_ptr == L'\0');
}

static inline TypedStringView::Type determineType(const std::wstring_view &src) noexcept
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
        if (src.compare(L"/*") == 0 || src.compare(L"*/") == 0)
            return TypedStringView::Type::MULTI_COMMENT;
        else if (src.compare(L"=+") == 0 || src.compare(L"+=") == 0)
            return TypedStringView::Type::ADD_EQUAL;
        else if (src.compare(L"=-") == 0 || src.compare(L"-=") == 0)
            return TypedStringView::Type::SUB_EQUAL;
        else if (src.compare(L"=/") == 0 || src.compare(L"/=") == 0)
            return TypedStringView::Type::SUB_EQUAL;
        else if (src.compare(L"++") == 0)
            return TypedStringView::Type::ADD_ONE;
        else if (src.compare(L"--") == 0)
            return TypedStringView::Type::SUB_ONE;
        else if (src.compare(L"or") == 0)
            return TypedStringView::Type::OR;
        else if (src.compare(L"->") == 0)
            return TypedStringView::Type::S_ARROW;
        else
            return TypedStringView::Type::IDENTIFIER;
    }
    break;
    case 3: {
        if (src.compare(L"ret") == 0)
            return TypedStringView::Type::RET;
        else if (src.compare(L"mod") == 0)
            return TypedStringView::Type::MODULO;
        else if (src.compare(L"and") == 0)
            return TypedStringView::Type::AND;
        else if (src.compare(L"xor") == 0)
            return TypedStringView::Type::XOR;
        else if (src.compare(L"ptr") == 0)
            return TypedStringView::Type::PTR;
        else if (src.compare(L"ref") == 0)
            return TypedStringView::Type::REF;
        else
            return TypedStringView::Type::IDENTIFIER;
    }
    break;
    case 4: {
        if (src.compare(L"    ") == 0)
            return TypedStringView::Type::TAB;
        else if (src.compare(L"func") == 0)
            return TypedStringView::Type::FUNC;
        else if (src.compare(L"int8") == 0)
            return TypedStringView::Type::INT8;
        else
            return TypedStringView::Type::IDENTIFIER;
    }
    break;
    case 5: {
        if (src.compare(L"uint8") == 0)
            return TypedStringView::Type::UINT8;
        else if (src.compare(L"int16") == 0)
            return TypedStringView::Type::INT16;
        else if (src.compare(L"int32") == 0)
            return TypedStringView::Type::INT32;
        else if (src.compare(L"int64") == 0)
            return TypedStringView::Type::INT64;
        else
            return TypedStringView::Type::IDENTIFIER;
    }
    break;
    case 6: {
        if (src.compare(L"string") == 0)
            return TypedStringView::Type::STRING;
        else if (src.compare(L"public") == 0)
            return TypedStringView::Type::PUBLIC_FUNC;
        else if (src.compare(L"global") == 0)
            return TypedStringView::Type::GLOBAL;
        else if (src.compare(L"module") == 0)
            return TypedStringView::Type::MODULE;
        else if (src.compare(L"import") == 0)
            return TypedStringView::Type::IMPORT;
        else if (src.compare(L"uint16") == 0)
            return TypedStringView::Type::UINT16;
        else if (src.compare(L"uint32") == 0)
            return TypedStringView::Type::UINT32;
        else if (src.compare(L"uint64") == 0)
            return TypedStringView::Type::UINT64;
        else
            return TypedStringView::Type::IDENTIFIER;
    }
    break;
    case 7: {
        if (src.compare(L"float32") == 0)
            return TypedStringView::Type::FLOAT32;
        else if (src.compare(L"float64") == 0)
            return TypedStringView::Type::FLOAT64;
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
    const std::vector<std::wstring_view> splits = split(src);
    size_t line = 0;
    bool toggle_push = false;
    for (std::wstring_view split : splits)
    {
        TypedStringView view(determineType(split), split, line);
        if (view.type == TypedStringView::Type::MULTI_COMMENT)
        {
            toggle_push ? toggle_push = false : toggle_push = true;
            continue;
        }
        if (view.type == TypedStringView::Type::HASH_TAG && !toggle_push)
        {
            toggle_push = true;
            continue;
        }
        if (view.type == TypedStringView::Type::NEW_LINE && toggle_push)
        {
            if (tokens.back().type == TypedStringView::Type::NEW_LINE)
                tokens.pop_back();
            if (toggle_push)
                toggle_push = false;
            line++;
            continue;
        }
        if (!toggle_push)
            tokens.push_back(view);
    }
    return tokens;
}
} // namespace LunaScript::parser::lexer