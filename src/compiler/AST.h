#pragma once
#include <vector>
#include <string>

typedef enum ASTTypeID
{
    AST_ROOT,
    AST_BLOCK,
    AST_FUNC_DEF,
    AST_FUNC_CALL,
    AST_EXPRESSION,
    AST_BINARY,
    AST_LITERAL,
} ASTTypeID;

typedef enum ASTExpressionType
{
    ASTE_RETURN,
    ASTE_VAR_DEFINED,
    ASTE_PRAM_LIST,
} ASTExpressionType;

typedef enum ASTDataType
{
    NOT_DETERMINED_TYPE,
    VOID_TYPE,
    INT8_TYPE,
    INT16_TYPE,
    INT32_TYPE,
    INT64_TYPE,
    UINT8_TYPE,
    UINT16_TYPE,
    UINT32_TYPE,
    UINT64_TYPE,
    FLOAT32_TYPE,
    FLOAT64_TYPE
} ASTDataType;

typedef enum ASTOperatorType
{
    ADD_TYPE,
    SUB_TYPE,
    DIV_TYPE,
    MUL_TYPE,
    AND_TYPE,
    OR_TYPE
} ASTOperatorType;

struct ASTNode
{
    public:
    virtual ASTTypeID getTypeID() const noexcept = 0;
};

struct ASTRoot : public ASTNode
{
    std::vector<const ASTNode *> children;
    ASTTypeID getTypeID() const noexcept override { return AST_ROOT;}
};

struct ASTExpression : public ASTNode
{
    std::vector<const ASTNode *> list;
    ASTExpressionType type;
    std::string extra_data = "";
    ASTTypeID getTypeID() const noexcept override { return AST_EXPRESSION;}
};

struct ASTBlock : public ASTNode
{
    std::vector<const ASTNode *> list;
    ASTTypeID getTypeID() const noexcept override { return AST_BLOCK;}
};

struct ASTFuncDef : public ASTNode
{
    const std::string name = "";
    const ASTExpression * args = nullptr;
    const ASTBlock *body = nullptr;
    ASTDataType return_type = NOT_DETERMINED_TYPE;
    ASTTypeID getTypeID() const noexcept override { return AST_FUNC_DEF;}
    explicit ASTFuncDef(const std::string name_in): name(name_in){}
};



struct ASTBinaryExpression : public ASTNode
{
    const ASTNode * right;
    const ASTNode * left;
    ASTOperatorType op;
    ASTTypeID getTypeID() const noexcept override { return AST_BINARY;}
};

struct ASTLiteral : public ASTNode
{
    std::string value = "";
    ASTDataType data_type = NOT_DETERMINED_TYPE;
    ASTTypeID getTypeID() const noexcept override { return AST_LITERAL;}
};