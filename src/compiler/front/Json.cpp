#include "../AST.h"
#include "Parser.h"
#include <rapidjson/prettywriter.h>
#include <rapidjson/stringbuffer.h>

namespace LunaScript::compiler::front
{
using namespace rapidjson;

const char *idToString(const NodeType &id) noexcept
{
    switch (id)
    {
    case NodeType::ROOT:
        return "Program";
    case NodeType::FUNC_DEF:
        return "FunctionDef";
    case NodeType::EXPRESSION:
        return "Expression";
    case NodeType::BINARY:
        return "BinaryExpression";
    case NodeType::LITERAL:
        return "Literal";
    case NodeType::BLOCK:
        return "ExecutionBlock";
    default:
        return "unknown";
    }
}

const char *idToStringT(const DataType &id) noexcept
{
    switch (id)
    {
    case DataType::NOT_DETERMINED:
        return "To Be Determined";
    case DataType::VOID:
        return "void";
    case DataType::INT8:
        return "int8";
    case DataType::INT16:
        return "int16";
    case DataType::INT32:
        return "int32";
    case DataType::INT64:
        return "int64";
    case DataType::UINT8:
        return "uint8";
    case DataType::UINT16:
        return "uint16";
    case DataType::UINT32:
        return "uint32";
    case DataType::UINT64:
        return "uint64";
    case DataType::FLOAT32:
        return "float32";
    case DataType::FLOAT64:
        return "float64";
    case DataType::ANY_FLOAT:
        return "any float";
    case DataType::ANY_UINT:
        return "any uint";
    case DataType::ANY_INT:
        return "any int";
    default:
        return "unknown";
    }
}

const char *idToStringE(const ExpressionType &id) noexcept
{
    switch (id)
    {
    case ExpressionType::RETURN:
        return "ReturnType";
    case ExpressionType::NO_RETURN:
        return "NoDataReturn";
    case ExpressionType::VAR_DEFINED:
        return "VariableDefinition";
    case ExpressionType::PRAM_LIST:
        return "ParameterList";
    case ExpressionType::FUNC_CALL:
        return "FunctionCall";
    default:
        return "unknown";
    }
}

const char *idToStringO(const OperatorType &id) noexcept
{
    switch (id)
    {
    case OperatorType::ADD:
        return "+";
    case OperatorType::SUB:
        return "-";
    case OperatorType::DIV:
        return "/";
    case OperatorType::MUL:
        return "*";
    case OperatorType::AND:
        return "and";
    case OperatorType::OR:
        return "or";
    case OperatorType::XOR:
        return "xor";
    case OperatorType::MOD:
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
    writer->String(idToString(node->getType()));
    switch (node->getType())
    {
    case NodeType::ROOT: {
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
    case NodeType::FUNC_DEF: {
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
    case NodeType::EXPRESSION: {
        const ASTExpression *real_node = static_cast<const ASTExpression *>(node);
        writer->Key("id");
        writer->String(idToStringE(real_node->type));
        if (real_node->data_type != DataType::NOT_DETERMINED)
        {
            writer->Key("data_type");
            writer->String(idToStringT(real_node->data_type));
        }
        if (!real_node->extra_data.empty())
        {
            writer->Key("name");
            writer->String(real_node->extra_data.c_str());
        }
        if (real_node->type == ExpressionType::FUNC_CALL)
        {
            const ASTFuncCall *func_node = static_cast<const ASTFuncCall *>(real_node);
            writer->Key("parameters");
            writeBranch(writer, func_node->args);
        }
        else
        {
            writer->Key("declarations");
            writer->StartArray();
            for (const ASTNode *child : real_node->list)
                writeBranch(writer, child);
            writer->EndArray();
        }
        break;
    }
    case NodeType::LITERAL: {
        const ASTLiteral *real_node = static_cast<const ASTLiteral *>(node);
        writer->Key("data_type");
        writer->String(idToStringT(real_node->data_type));
        writer->Key("value");
        writer->String(real_node->value.c_str());
        break;
    }
    case NodeType::BINARY: {
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
    case NodeType::BLOCK: {
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
} // namespace LunaScript::compiler::front