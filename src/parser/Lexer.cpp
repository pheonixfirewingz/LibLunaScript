#include "Lexer.h"
#include <iostream>
namespace LunaScript::parser::lexer
{
std::vector<LexicalTypedSpan> tokenize(const std::unicodestring &)
{
    std::vector<LexicalTypedSpan> spans;

    return spans;
}

static const char* toString(LexicalTypedSpan::Type type)
{
    switch (type)
    {
    case LunaScript::parser::lexer::LexicalTypedSpan::Type::IMPORT:
        return "IMPORT";
    case LunaScript::parser::lexer::LexicalTypedSpan::Type::MODULE_NAME:
        return "MODULE_NAME";
    case LunaScript::parser::lexer::LexicalTypedSpan::Type::IDENTIFIER:
        return "IDENTIFIER";
    case LunaScript::parser::lexer::LexicalTypedSpan::Type::FUNC:
        return "FUNC";
    case LunaScript::parser::lexer::LexicalTypedSpan::Type::PUBLIC_FUNC:
        return "PUBLIC_FUNC";
    case LunaScript::parser::lexer::LexicalTypedSpan::Type::RET:
        return "RET";
    case LunaScript::parser::lexer::LexicalTypedSpan::Type::R_SQUIGGLY:
        return "R_SQUIGGLY";
    case LunaScript::parser::lexer::LexicalTypedSpan::Type::L_SQUIGGLY:
        return "L_SQUIGGLY";
    case LunaScript::parser::lexer::LexicalTypedSpan::Type::R_CURLY:
        return "R_CURLY";
    case LunaScript::parser::lexer::LexicalTypedSpan::Type::L_CURLY:
        return "L_CURLY";
    case LunaScript::parser::lexer::LexicalTypedSpan::Type::COMMA:
        return "COMMA";
    case LunaScript::parser::lexer::LexicalTypedSpan::Type::DOT:
        return "DOT";
    case LunaScript::parser::lexer::LexicalTypedSpan::Type::ADD:
        return "ADD";
    case LunaScript::parser::lexer::LexicalTypedSpan::Type::SUB:
        return "SUB";
    case LunaScript::parser::lexer::LexicalTypedSpan::Type::MUL:
        return "MUL";
    case LunaScript::parser::lexer::LexicalTypedSpan::Type::DIV:
        return "DIV";
    case LunaScript::parser::lexer::LexicalTypedSpan::Type::AND:
        return "AND";
    case LunaScript::parser::lexer::LexicalTypedSpan::Type::OR:
        return "OR";
    case LunaScript::parser::lexer::LexicalTypedSpan::Type::XOR:
        return "XOR";
    case LunaScript::parser::lexer::LexicalTypedSpan::Type::MODULO:
        return "MODULO";
    case LunaScript::parser::lexer::LexicalTypedSpan::Type::EQUAL:
        return "EQUAL";
    case LunaScript::parser::lexer::LexicalTypedSpan::Type::R_ARROW:
        return "R_ARROW";
    case LunaScript::parser::lexer::LexicalTypedSpan::Type::L_ARROW:
        return "L_ARROW";
    case LunaScript::parser::lexer::LexicalTypedSpan::Type::S_ARROW:
        return "SUPER_ARROW";
    case LunaScript::parser::lexer::LexicalTypedSpan::Type::NO_RETURN:
        return "NO_RETURN";
    case LunaScript::parser::lexer::LexicalTypedSpan::Type::GLOBAL:
        return "GLOBAL";
    case LunaScript::parser::lexer::LexicalTypedSpan::Type::INT8:
        return "INT8";
    case LunaScript::parser::lexer::LexicalTypedSpan::Type::INT16:
        return "INT16";
    case LunaScript::parser::lexer::LexicalTypedSpan::Type::INT32:
        return "INT32";
    case LunaScript::parser::lexer::LexicalTypedSpan::Type::INT64:
        return "INT64";
    case LunaScript::parser::lexer::LexicalTypedSpan::Type::UINT8:
        return "UINT8";
    case LunaScript::parser::lexer::LexicalTypedSpan::Type::UINT16:
        return "UINT16";
    case LunaScript::parser::lexer::LexicalTypedSpan::Type::UINT32:
        return "UINT32";
    case LunaScript::parser::lexer::LexicalTypedSpan::Type::UINT64:
        return "UINT64";
    case LunaScript::parser::lexer::LexicalTypedSpan::Type::FLOAT32:
        return "FLOAT32";
    case LunaScript::parser::lexer::LexicalTypedSpan::Type::FLOAT64:
        return "FLOAT64";
    case LunaScript::parser::lexer::LexicalTypedSpan::Type::STRING:
        return "STRING";
    case LunaScript::parser::lexer::LexicalTypedSpan::Type::LETTER:
        return "LETTER";
    case LunaScript::parser::lexer::LexicalTypedSpan::Type::NEW_LINE:
        return "NEW_LINE";
    case LunaScript::parser::lexer::LexicalTypedSpan::Type::TAB:
        return "TAB";
    case LunaScript::parser::lexer::LexicalTypedSpan::Type::SPACE:
        return "SPACE";
    default:
        return "ERROR";
    }
}

void LexicalTypedSpan::print(std::unicodestring)
{
    std::wcout << "(TOKEN: {TYPE: ";
    std::wcout  << toString(type);
    std::wcout << ", STRING: ";
    for (auto c : span)
    {
        std::cout << static_cast<char>(c);
    }
    std::wcout << "\"})";
}
} // namespace LunaScript::parser::lexer