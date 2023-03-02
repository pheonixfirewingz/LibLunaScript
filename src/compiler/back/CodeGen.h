#pragma once
#include "../AST.h"

namespace LunaScript::compiler::back
{
using namespace LunaScript::compiler::ast;
class CodeGenerator
{
    const ASTRoot *root;
    std::string ret;
    void error();

    struct BinaryInfo
    {
        const uint16_t precedence;
        const OperatorType op;
        const ASTLiteral *left;
        const ASTLiteral *right;
        BinaryInfo(const uint16_t precedence_in, const OperatorType op_in, const ASTLiteral *left_in,
                   const ASTLiteral *right_in)
            : precedence(precedence_in)
            , op(op_in)
            , left(left_in)
            , right(right_in)
        {
        }
    };

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

    void genFunction(const ASTFuncDef *);
    void genBinary(const ASTBinaryExpression *);
    std::string genLiteral(const uint8_t,const ASTLiteral *,bool global = false);

  public:
    explicit CodeGenerator(const ASTRoot *root)
        : root(root)
    {
    }

    std::string generate();
};
} // namespace LunaScript::compiler::back