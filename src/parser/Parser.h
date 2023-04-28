#pragma once
#include "../AST.h"
#include "Lexer.h"
namespace LunaScript::parser
{
using namespace LunaScript::parser::lexer;
using namespace LunaScript::compiler::ast;
struct Parser
{
    Parser() = default;
    ~Parser() = default;
    ASTRoot *operator()(const std::vector<TypedStringView>& input) const;
};
} // namespace LunaScript::parser