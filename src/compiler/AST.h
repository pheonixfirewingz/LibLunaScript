#pragma once
#include <string>
#include <vector>
#include <extend_std/Vector.h>

namespace LunaScript::compiler::ast
{

enum class NodeType
{
    ROOT,
    BLOCK,
    FUNC_DEF,
    EXPRESSION
};

enum class ExpressionType
{
    RETURN,
    NO_RETURN,
    VAR_DEFINED,
    PRAM_LIST,
    FUNC_CALL,
    BINARY,
    LITERAL
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
    ANY_INT
};

enum class OperatorType
{
    NOT_DETERMINED,
    ADD,
    SUB,
    MUL,
    DIV,
    MOD,
    AND,
    XOR,
    OR
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
        : name(name)
    {
    }

    virtual ~ASTRoot()
    {
        children.clear();
    }
};

struct ASTExpression : public ASTNode
{
    virtual ExpressionType getExprType() const noexcept = 0;
    NodeType getType() const noexcept override
    {
        return NodeType::EXPRESSION;
    }
    ASTExpression() = default;
    virtual ~ASTExpression() = default;
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
    std::string func_name = "";
    explicit ASTFuncCall(std::string name_in, const ASTExpression *args_in)
        : func_name(name_in)
        , args(args_in)
    {
    }

    virtual ExpressionType getExprType() const noexcept override
    {
        return ExpressionType::FUNC_CALL;
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

struct ASTNoReturnExpression : public ASTExpression
{
    virtual ExpressionType getExprType() const noexcept override
    {
        return ExpressionType::NO_RETURN;
    }
};

struct ASTParamListExpression : public ASTExpression
{
    const std::ReadOnlyVector<const ASTExpression *> prams;
    ASTParamListExpression(const std::ReadOnlyVector<const ASTExpression *> prams_in)
        : prams(prams_in)
    {
    }
    virtual ExpressionType getExprType() const noexcept override
    {
        return ExpressionType::PRAM_LIST;
    }
};

struct ASTReturnExpression : public ASTExpression
{
    DataType return_type = DataType::NOT_DETERMINED;
    const ASTExpression *child = nullptr;
    virtual ExpressionType getExprType() const noexcept override
    {
        return ExpressionType::RETURN;
    }

    ~ASTReturnExpression()
    {
        if (child)
            delete child;
    }
};

struct ASTLiteral : public ASTExpression
{
    bool global = false;
    std::string name = "";
    std::string value = "";
    DataType data_type = DataType::NOT_DETERMINED;
    const ASTExpression* child = nullptr;

    bool hasChild() const noexcept
    {
        return child != nullptr;
    }

    virtual ExpressionType getExprType() const noexcept override
    {
        return name.empty() ? ExpressionType::LITERAL : ExpressionType::VAR_DEFINED;
    }

    virtual ~ASTLiteral() = default;
};

struct ASTBinaryExpression : public ASTExpression
{
    uint16_t precedence = 0;
    const ASTExpression *right = nullptr;
    const ASTLiteral *left = nullptr;
    OperatorType op = OperatorType::NOT_DETERMINED;
    
    virtual ExpressionType getExprType() const noexcept override
    {
        return ExpressionType::BINARY;
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