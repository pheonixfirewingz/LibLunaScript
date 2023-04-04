#include "Lexer.h"
#include <iostream>
namespace LunaScript::parser::lexer
{

static const char *toString(TypedStringView::Type type)
{
    switch (type)
    {
    case LunaScript::parser::lexer::TypedStringView::Type::IMPORT:
        return "IMPORT";
    case LunaScript::parser::lexer::TypedStringView::Type::MODULE_NAME:
        return "MODULE_NAME";
    case LunaScript::parser::lexer::TypedStringView::Type::IDENTIFIER:
        return "IDENTIFIER";
    case LunaScript::parser::lexer::TypedStringView::Type::FUNC:
        return "FUNC";
    case LunaScript::parser::lexer::TypedStringView::Type::PUBLIC_FUNC:
        return "PUBLIC_FUNC";
    case LunaScript::parser::lexer::TypedStringView::Type::RET:
        return "RET";
    case LunaScript::parser::lexer::TypedStringView::Type::R_SQUIGGLY:
        return "R_SQUIGGLY";
    case LunaScript::parser::lexer::TypedStringView::Type::L_SQUIGGLY:
        return "L_SQUIGGLY";
    case LunaScript::parser::lexer::TypedStringView::Type::R_CURLY:
        return "R_CURLY";
    case LunaScript::parser::lexer::TypedStringView::Type::L_CURLY:
        return "L_CURLY";
    case LunaScript::parser::lexer::TypedStringView::Type::COMMA:
        return "COMMA";
    case LunaScript::parser::lexer::TypedStringView::Type::DOT:
        return "DOT";
    case LunaScript::parser::lexer::TypedStringView::Type::ADD:
        return "ADD";
    case LunaScript::parser::lexer::TypedStringView::Type::SUB:
        return "SUB";
    case LunaScript::parser::lexer::TypedStringView::Type::MUL:
        return "MUL";
    case LunaScript::parser::lexer::TypedStringView::Type::DIV:
        return "DIV";
    case LunaScript::parser::lexer::TypedStringView::Type::AND:
        return "AND";
    case LunaScript::parser::lexer::TypedStringView::Type::OR:
        return "OR";
    case LunaScript::parser::lexer::TypedStringView::Type::XOR:
        return "XOR";
    case LunaScript::parser::lexer::TypedStringView::Type::MODULO:
        return "MODULO";
    case LunaScript::parser::lexer::TypedStringView::Type::EQUAL:
        return "EQUAL";
    case LunaScript::parser::lexer::TypedStringView::Type::R_ARROW:
        return "R_ARROW";
    case LunaScript::parser::lexer::TypedStringView::Type::L_ARROW:
        return "L_ARROW";
    case LunaScript::parser::lexer::TypedStringView::Type::S_ARROW:
        return "SUPER_ARROW";
    case LunaScript::parser::lexer::TypedStringView::Type::NO_RETURN:
        return "NO_RETURN";
    case LunaScript::parser::lexer::TypedStringView::Type::GLOBAL:
        return "GLOBAL";
    case LunaScript::parser::lexer::TypedStringView::Type::INT8:
        return "INT8";
    case LunaScript::parser::lexer::TypedStringView::Type::INT16:
        return "INT16";
    case LunaScript::parser::lexer::TypedStringView::Type::INT32:
        return "INT32";
    case LunaScript::parser::lexer::TypedStringView::Type::INT64:
        return "INT64";
    case LunaScript::parser::lexer::TypedStringView::Type::UINT8:
        return "UINT8";
    case LunaScript::parser::lexer::TypedStringView::Type::UINT16:
        return "UINT16";
    case LunaScript::parser::lexer::TypedStringView::Type::UINT32:
        return "UINT32";
    case LunaScript::parser::lexer::TypedStringView::Type::UINT64:
        return "UINT64";
    case LunaScript::parser::lexer::TypedStringView::Type::FLOAT32:
        return "FLOAT32";
    case LunaScript::parser::lexer::TypedStringView::Type::FLOAT64:
        return "FLOAT64";
    case LunaScript::parser::lexer::TypedStringView::Type::STRING:
        return "STRING";
    case LunaScript::parser::lexer::TypedStringView::Type::LETTER:
        return "LETTER";
    case LunaScript::parser::lexer::TypedStringView::Type::NEW_LINE:
        return "NEW_LINE";
    case LunaScript::parser::lexer::TypedStringView::Type::TAB:
        return "TAB";
    case LunaScript::parser::lexer::TypedStringView::Type::SPACE:
        return "SPACE";
    default:
        return "ERROR";
    }
}

void TypedStringView::print(std::wstring)
{
    std::wcout << L"(TOKEN: {TYPE: " << toString(type)  << L", STRING: \"";
    std::wcout << span << L"\", LINE: " << line << L"})\n";
}

constexpr inline bool Lexer::isUnicode(const wchar_t &c) noexcept
{
    return (c & 128) != 0;
}

const std::vector<std::wstring_view> Lexer::split(const std::wstring &str, const wchar_t delimiter)
{
    std::vector<std::wstring_view> result;
    std::wstring::size_type start = 0;
    std::wstring::size_type end = str.find(delimiter);
    while (end != std::wstring::npos)
    {
        result.push_back(str.substr(start, (end - start) + 1));
        start = end + 1;
        end = str.find(delimiter, start);
    }
    result.push_back(std::wstring_view(&str[start]));
    return result;
}

std::vector<TypedStringView> Lexer::operator()(const std::wstring &src) const
{
    std::vector<TypedStringView> tokens;
    const std::vector<std::wstring_view> lines = split(src, L'\n');
    for (size_t i = 0; i < lines.size(); i++)
    {
        std::wstring_view line = lines[i];
        std::wcout << line;
    }
    return tokens;
}
} // namespace LunaScript::parser::lexer