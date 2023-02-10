#pragma once
#include "../AST.h"

namespace LunaScript::compiler::back
{
using namespace LunaScript::compiler::ast;
class CodeGenerator
{
    constexpr static const char *tab_str = "    ";
    const ASTRoot *root;
    void error();

    bool isIDaNumeric(const DataType &id) noexcept
    {
        switch (id)
        {
        case DataType::INT8:
        case DataType::INT16:
        case DataType::INT32:
        case DataType::INT64:
        case DataType::UINT8:
        case DataType::UINT16:
        case DataType::UINT32:
        case DataType::UINT64:
        case DataType::FLOAT32:
        case DataType::FLOAT64:
        case DataType::ANY_FLOAT:
        case DataType::ANY_UINT:
        case DataType::ANY_INT:
            return true;
        default:
            return false;
        }
    }

    std::string genFunction(const ASTFuncDef *);
    std::string genBinary(const ASTBinaryExpression *);

  public:
    explicit CodeGenerator(const ASTRoot *root)
        : root(root)
    {
    }

    std::string generate();
};
} // namespace LunaScript::compiler::back