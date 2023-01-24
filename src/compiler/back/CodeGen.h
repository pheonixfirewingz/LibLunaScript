#pragma once
#include "../AST.h"

namespace LunaScript::compiler::back
{
using namespace LunaScript::compiler::ast;
class CodeGenerator
{
    const ASTRoot *root;
    void error();
    std::string genFunction(const ASTFuncDef *);

  public:
    explicit CodeGenerator(const ASTRoot *root)
        : root(root)
    {
    }

    std::string generate();
};
} // namespace LunaScript::compiler::back