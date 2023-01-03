#include "../AST.h"
#include "Parser.h"
#include <rapidjson/prettywriter.h>
#include <rapidjson/stringbuffer.h>

namespace LunaScript::compiler::front
{
using namespace rapidjson;

const char *idToString(const ASTTypeID &id) noexcept
{
    switch (id)
    {
    case AST_ROOT:
        return "Program";
    case AST_FUNC_DEF:
        return "FunctionDef";
    case AST_FUNC_CALL:
        return "FunctionCall";
    case AST_EXPRESSION:
        return "Expression";
    case AST_BINARY:
        return "BinaryExpression";
    case AST_LITERAL:
        return "Literal";
    case AST_BLOCK:
        return "ExecutionBlock";
    default:
        return "unknown";
    }
}

const char *idToStringT(const ASTDataType &id) noexcept
{
    switch (id)
    {
    case NOT_DETERMINED_DATA_TYPE:
        return "To Be Determined";
    case VOID_TYPE:
        return "Void";
    case INT8_TYPE:
        return "int8";
    case INT16_TYPE:
        return "int16";
    case INT32_TYPE:
        return "int32";
    case INT64_TYPE:
        return "int64";
    case UINT8_TYPE:
        return "uint8";
    case UINT16_TYPE:
        return "uint16";
    case UINT32_TYPE:
        return "uint32";
    case UINT64_TYPE:
        return "uint64";
    case FLOAT32_TYPE:
        return "float32";
    case FLOAT64_TYPE:
        return "float64";
    case FLOAT_ANY_TYPE:
        return "any float";
    case UINT_ANY_TYPE:
        return "any uint";
    case INT_ANY_TYPE:
        return "any int";
    default:
        return "unknown";
    }
}

const char *idToStringE(const ASTExpressionType &id) noexcept
{
    switch (id)
    {
    case AST_EXPR_RETURN:
        return "ReturnType";
    case AST_EXPR_NO_RETURN:
        return "NoDataReturn";
    case AST_EXPR_VAR_DEFINED:
        return "VariableDefinition";
    case AST_EXPR_PRAM_LIST:
        return "ParameterList";
    default:
        return "unknown";
    }
}

const char *idToStringO(const ASTOperatorType &id) noexcept
{
    switch (id)
    {
    case ADD_TYPE:
        return "+";
    case SUB_TYPE:
        return "-";
    case DIV_TYPE:
        return "/";
    case MUL_TYPE:
        return "*";
    case AND_TYPE:
        return "and";
    case OR_TYPE:
        return "or";
    case XOR_TYPE:
        return "xor";
    case MOD_TYPE:
        return "modulo";
    default:
        return "unknown";
    }
}
template<typename T> void writeBranch(T *writer, const ASTNode *node)
{
    if (node == nullptr)
    {
        writer->StartObject();
        writer->EndObject();
        return;
    }
    writer->StartObject();
    writer->Key("type");
    writer->String(idToString(node->getTypeID()));
    switch (node->getTypeID())
    {
    case AST_ROOT: {
        const ASTRoot *real_node = static_cast<const ASTRoot *>(node);
        writer->Key("body");
        writer->StartArray();
        for (const ASTNode *child : real_node->children)
            writeBranch(writer, child);
        writer->EndArray();
        writer->Key("module");
        writer->String(real_node->name.c_str());
        break;
    }
    case AST_FUNC_DEF: {
        const ASTFuncDef *real_node = static_cast<const ASTFuncDef *>(node);
        writer->Key("is_public");
        writer->Bool(real_node->is_public);
        writer->Key("name");
        writer->String(real_node->name.c_str());
        writer->Key("return_type");
        writer->String(idToStringT(real_node->return_type));
        writer->Key("parameters");
        writeBranch(writer, real_node->args);
        writer->Key("body");
        writeBranch(writer, real_node->body);
        break;
    }
    case AST_FUNC_CALL: {
    }
    case AST_EXPRESSION: {
        const ASTExpression *real_node = static_cast<const ASTExpression *>(node);
        writer->Key("id");
        writer->String(idToStringE(real_node->type));
        if (real_node->data_type != NOT_DETERMINED_DATA_TYPE)
        {
            writer->Key("data_type");
            writer->String(idToStringT(real_node->data_type));
        }
        if (!real_node->extra_data.empty())
        {
            writer->Key("name");
            writer->String(real_node->extra_data.c_str());
        }
        writer->Key("declarations");
        writer->StartArray();
        for (const ASTNode *child : real_node->list)
            writeBranch(writer, child);
        writer->EndArray();
        break;
    }
    case AST_LITERAL: {
        const ASTLiteral *real_node = static_cast<const ASTLiteral *>(node);
        writer->Key("data_type");
        writer->String(idToStringT(real_node->data_type));
        writer->Key("value");
        writer->String(real_node->value.c_str());
        break;
    }
    case AST_BINARY: {
        const ASTBinaryExpression *real_node = static_cast<const ASTBinaryExpression *>(node);
        writer->Key("precedence");
        writer->Uint(real_node->precedence);
        writer->Key("op");
        writer->String(idToStringO(real_node->op));
        writer->Key("right");
        writeBranch(writer, real_node->right);
        writer->Key("left");
        writeBranch(writer, real_node->left);
        break;
    }
    case AST_BLOCK: {
        const ASTBlock *real_node = static_cast<const ASTBlock *>(node);
        writer->Key("execution");
        writer->StartArray();
        for (const ASTNode *child : real_node->list)
            writeBranch(writer, child);
        writer->EndArray();
    }
    default:
        break;
    }
    writer->EndObject();
}

const std::string Parser::asString(const bool pretty_mode) noexcept
{
    StringBuffer s;
    if (pretty_mode)
    {
        PrettyWriter<StringBuffer> writer(s);
        writeBranch<PrettyWriter<StringBuffer>>(&writer, root);
    }
    else
    {
        Writer<StringBuffer> writer(s);
        writeBranch<Writer<StringBuffer>>(&writer, root);
    }
    return s.GetString();
}
} // namespace LunaScript::compiler