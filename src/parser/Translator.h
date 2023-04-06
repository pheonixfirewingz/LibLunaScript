#pragma once
#include <libos/DataType.h>
#include <string>
namespace LunaScript::parser::lexer
{

struct Translator
{
    Translator() = default;
    ~Translator() = default;
    std::wstring operator()(const std::wstring input) const;
};

} // namespace LunaScript::parser::lexer