#pragma once
#include <extend_std/Vector.h>
#include <string>
#include <vector>

namespace LunaScript::compiler::ast
{

enum class NodeType
{
    ROOT,
    MODULE,
    BLOCK,
    FUNC_DEF,
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

struct ASTModule: public ASTNode
{
    const std::string name;
    std::vector<const ASTNode *> children;

    explicit ASTModule(const std::string module_name) : name(module_name) {}

    NodeType getType() const noexcept override
    {
        return NodeType::MODULE;
    }

    virtual ~ASTModule()
    {
        children.clear();
    }
};

struct ASTRoot : public ASTNode
{
    std::vector<const ASTModule *> modules;
    NodeType getType() const noexcept override
    {
        return NodeType::ROOT;
    }

    virtual ~ASTRoot()
    {
        modules.clear();
    }
};

struct ASTBlock : public ASTNode
{
    std::vector<const ASTNode *> list;
    NodeType getType() const noexcept override
    {
        return NodeType::BLOCK;
    }
    virtual ~ASTBlock()
    {
        list.clear();
    }
};

struct ASTParamListExpression : public ASTNode
{
    const std::ReadOnlyVector<const ASTNode *> prams;
    ASTParamListExpression(const std::ReadOnlyVector<const ASTNode *> prams_in)
        : prams(prams_in)
    {
    }
    NodeType getType() const noexcept override
    {
        return NodeType::PRAM_LIST;
    }
};

struct ASTFuncCall : public ASTNode
{
    const ASTParamListExpression *args;
    std::string func_name = "";
    explicit ASTFuncCall(std::string name_in, const ASTParamListExpression *args_in)
        : args(args_in)
        , func_name(name_in)
    {
    }

    virtual NodeType getType() const noexcept override
    {
        return NodeType::FUNC_CALL;
    }

    virtual ~ASTFuncCall()
    {
        if (args)
            delete args;
    }
};

struct ASTFuncDef : public ASTNode
{
    std::string name = "";
    bool is_public = false;
    ASTParamListExpression *args = nullptr;
    ASTBlock *body = nullptr;
    DataType return_type = DataType::NOT_DETERMINED;
    NodeType getType() const noexcept override
    {
        return NodeType::FUNC_DEF;
    }
    ASTFuncDef(std::string name_in, bool is_public_in)
        : name(name_in)
        , is_public(is_public_in)
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

struct ASTNoReturnExpression : public ASTNode
{
    NodeType getType() const noexcept override
    {
        return NodeType::NO_RETURN;
    }
};

struct ASTReturnExpression : public ASTNode
{
    DataType return_type = DataType::NOT_DETERMINED;
    const ASTNode *child = nullptr;
    NodeType getType() const noexcept override
    {
        return NodeType::RETURN;
    }

    ~ASTReturnExpression()
    {
        if (child)
            delete child;
    }
};

struct ASTVarDef : public ASTNode
{
    bool global = false;
    std::string name = "";
    std::string value = "";
    DataType data_type = DataType::NOT_DETERMINED;
    const ASTNode *child = nullptr;

    bool hasChild() const noexcept
    {
        return child != nullptr;
    }

    NodeType getType() const noexcept override
    {
        return NodeType::VAR_DEFINED;
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
};

struct ASTBinaryExpression : public ASTNode
{
    uint16_t precedence = 0;
    const ASTNode *right = nullptr;
    const ASTNode *left = nullptr;
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