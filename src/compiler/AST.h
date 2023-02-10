#pragma once
#include <string>
#include <vector>

namespace LunaScript::compiler::ast
{

enum class NodeType
{
    ROOT,
    BLOCK,
    FUNC_DEF,
    EXPRESSION,
    BINARY,
    LITERAL
};

enum class ExpressionType
{
    RETURN,
    NO_RETURN,
    VAR_DEFINED,
    PRAM_LIST,
    FUNC_CALL
};

enum class DataType
{
    NOT_DETERMINED,
    VOID,
    INT8,
    INT16,
    INT32,
    INT64,
    UINT8,
    UINT16,
    UINT32,
    UINT64,
    FLOAT32,
    FLOAT64,
    ANY_FLOAT,
    ANY_UINT,
    ANY_INT,
};

enum class OperatorType
{
    NOT_DETERMINED,
    ADD,
    SUB,
    DIV,
    MUL,
    AND,
    OR,
    XOR,
    MOD
};

struct ASTNode
{
  public:
    virtual NodeType getType() const noexcept = 0;
    virtual ~ASTNode() = default;
};

struct ASTRoot : public ASTNode
{
    std::string name;
    std::vector<const ASTNode *> children;
    NodeType getType() const noexcept override
    {
        return NodeType::ROOT;
    }

    ASTRoot(const std::string name) noexcept
        : name(std::move(name))
    {
    }

    virtual ~ASTRoot()
    {
        children.clear();
    }
};

struct ASTExpression : public ASTNode
{
    std::vector<const ASTNode *> list;
    ExpressionType type;
    DataType data_type = DataType::NOT_DETERMINED;
    std::string extra_data = "";
    NodeType getType() const noexcept override
    {
        return NodeType::EXPRESSION;
    }
    ASTExpression() = default;
    ASTExpression(std::string name_in, ExpressionType type_in)
        : type(type_in)
        , extra_data(name_in)
    {
    }

    virtual ~ASTExpression()
    {
        list.clear();
    }
};

struct ASTBlock : public ASTNode
{
    std::vector<const ASTExpression *> list;
    NodeType getType() const noexcept override
    {
        return NodeType::BLOCK;
    }
    virtual ~ASTBlock()
    {
        list.clear();
    }
};

struct ASTFuncCall : public ASTExpression
{
    const ASTExpression *args;
    explicit ASTFuncCall(std::string name_in, const ASTExpression *args_in)
        : ASTExpression(name_in, ExpressionType::FUNC_CALL)
        , args(args_in)
    {
    }

    NodeType getType() const noexcept override
    {
        return ASTExpression::getType();
    }

    virtual ~ASTFuncCall()
    {
        if (args)
            delete args;
    }
};

struct ASTFuncDef : public ASTNode
{
    const std::string name = "";
    const bool is_public;
    const ASTExpression *args;
    ASTBlock *body = nullptr;
    DataType return_type = DataType::NOT_DETERMINED;
    NodeType getType() const noexcept override
    {
        return NodeType::FUNC_DEF;
    }
    explicit ASTFuncDef(const std::string name_in, bool is_public)
        : name(name_in)
        , is_public(is_public)
    {
    }
    virtual ~ASTFuncDef()
    {
        if (args)
            delete args;
        if (body)
            delete body;
    }
};

struct ASTLiteral : public ASTNode
{
    std::string value = "";
    DataType data_type = DataType::NOT_DETERMINED;
    NodeType getType() const noexcept override
    {
        return NodeType::LITERAL;
    }

    virtual ~ASTLiteral() = default;
};

struct ASTBinaryExpression : public ASTNode
{
    uint16_t precedence = 0;
    const ASTNode *right = nullptr;
    const ASTLiteral *left = nullptr;
    OperatorType op = OperatorType::NOT_DETERMINED;
    NodeType getType() const noexcept override
    {
        return NodeType::BINARY;
    }
    virtual ~ASTBinaryExpression()
    {
        if (left)
            delete left;
        if (right)
            delete right;
    }
};
} // namespace LunaScript::compiler::ast