#include "../AST.h"
#include <rapidjson/prettywriter.h>
#include <rapidjson/stringbuffer.h>
namespace LunaScript::compiler::front
{
using namespace LunaScript::compiler::ast;
using namespace rapidjson;
#ifdef WIN32
std::string ReplaceAll(std::string str, const std::string &from, const std::string &to)
{
    size_t start_pos = 0;
    while ((start_pos = str.find(from, start_pos)) != std::string::npos)
    {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length();
    }
    return str;
}
#endif //  WIN32

const wchar_t *idToString(const NodeType &id) noexcept
{
    switch (id)
    {
    case NodeType::ROOT:
        return L"program";
    case NodeType::MODULE:
        return L"module";
    case NodeType::BLOCK:
        return L"execution_block";
    case NodeType::FUNC_DEF:
        return L"function_definition";
    case NodeType::RETURN:
        return L"data_return";
    case NodeType::NO_RETURN:
        return L"no_data_return";
    case NodeType::VAR_DEFINED:
        return L"variable_definition";
    case NodeType::PRAM_LIST:
        return L"parameter_list";
    case NodeType::FUNC_CALL:
        return L"function_call";
    case NodeType::BINARY:
        return L"binary_operation";
    case NodeType::LITERAL:
        return L"literal";
    default:
        return L"unknown";
    }
}

const wchar_t *idToStringT(const DataType &id) noexcept
{
    switch (id)
    {
    case DataType::NOT_DETERMINED:
        return L"to_be_determined";
    case DataType::VOID:
        return L"void";
    case DataType::INT8:
        return L"int8";
    case DataType::INT16:
        return L"int16";
    case DataType::INT32:
        return L"int32";
    case DataType::INT64:
        return L"int64";
    case DataType::UINT8:
        return L"uint8";
    case DataType::UINT16:
        return L"uint16";
    case DataType::UINT32:
        return L"uint32";
    case DataType::UINT64:
        return L"uint64";
    case DataType::FLOAT32:
        return L"float32";
    case DataType::FLOAT64:
        return L"float64";
    case DataType::ANY_FLOAT:
        return L"any_float";
    case DataType::ANY_UINT:
        return L"any_uint";
    case DataType::ANY_INT:
        return L"any_int";
    default:
        return L"unknown";
    }
}

const wchar_t *idToStringO(const OperatorType &id) noexcept
{
    switch (id)
    {
    case OperatorType::ADD:
        return L"+";
    case OperatorType::SUB:
        return L"-";
    case OperatorType::DIV:
        return L"/";
    case OperatorType::MUL:
        return L"*";
    case OperatorType::AND:
        return L"and";
    case OperatorType::OR:
        return L"or";
    case OperatorType::XOR:
        return L"xor";
    case OperatorType::MOD:
        return L"modulo";
    default:
        return L"unknown";
    }
}
void writeBranch(PrettyWriter<GenericStringBuffer<UTF16<>>, UTF16<>> *writer, const ASTNode *node)
{
    if (node == nullptr)
    {
        writer->StartObject();
        writer->EndObject();
        return;
    }
    writer->StartObject();
    writer->Key(L"type");
    writer->String(idToString(node->getType()));
    switch (node->getType())
    {
    case NodeType::ROOT: {
        const auto *real_node = static_cast<const ASTRoot *>(node);
        writer->Key(L"modules");
        writer->StartArray();
        for (const ASTNode *child : real_node->modules)
            writeBranch(writer, child);
        writer->EndArray();
        break;
    }
    case NodeType::MODULE: {
        const auto *real_node = static_cast<const ASTModule *>(node);
        writer->Key(L"name");
        writer->String(real_node->name.c_str());
        writer->Key(L"data");
        writer->StartArray();
        for (const ASTNode *child : real_node->children)
            writeBranch(writer, child);
        writer->EndArray();
    }
    break;
    case NodeType::FUNC_DEF: {
        const auto *real_node = static_cast<const ASTFuncDef *>(node);
        writer->Key(L"is_public");
        writer->Bool(real_node->is_public);
        writer->Key(L"name");
        writer->String(real_node->name.c_str());
        writer->Key(L"return_type");
        writer->String(idToStringT(real_node->return_type));
        writer->Key(L"parameters");
        writeBranch(writer, real_node->args);
        writer->Key(L"body");
        writeBranch(writer, real_node->body);
    }
    break;
    case NodeType::RETURN: {
        writer->Key(L"declaration");
        const auto *cast = static_cast<const ASTReturnExpression *>(node);
        writeBranch(writer, static_cast<const ASTNode *>(cast->child));
    }
    break;
    case NodeType::FUNC_CALL: {
        const auto *cast = static_cast<const ASTFuncCall *>(node);
        writer->Key(L"name");
        writer->String(cast->func_name.c_str());
        writer->Key(L"parameters");
        writeBranch(writer, cast->args);
    }
    break;
    case NodeType::VAR_DEFINED: {
        const auto *cast = static_cast<const ASTVarDef *>(node);
        writer->Key(L"data_type");
        writer->String(idToStringT(cast->data_type));
        writer->Key(L"is_global");
        writer->Bool(cast->global);
        writer->Key(L"name");
        writer->String(cast->name.c_str());
        if (cast->hasChild())
        {
            writer->Key(L"declaration");
            writeBranch(writer, static_cast<const ASTNode *>(cast->child));
        }
        else
        {
            writer->Key(L"value");
            writer->String(cast->value.c_str());
        }
    }
    break;
    case NodeType::LITERAL: {
        const auto *cast = static_cast<const ASTLiteral *>(node);
        writer->Key(L"data_type");
        writer->String(idToStringT(cast->data_type));
        writer->Key(L"value");
        writer->String(cast->value.c_str());
    }
    break;
    case NodeType::PRAM_LIST: {
        const auto *cast = static_cast<const ASTParamListExpression *>(node);
        writer->Key(L"parameters");
        writer->StartArray();
        for (const ASTNode *pram : cast->prams)
            writeBranch(writer, pram);
        writer->EndArray();
    }
    break;
    case NodeType::BINARY: {
        const auto *real_node = static_cast<const ASTBinaryExpression *>(node);
        writer->Key(L"precedence");
        writer->Uint(real_node->precedence);
        writer->Key(L"op");
        writer->String(idToStringO(real_node->op));
        writer->Key(L"right");
        writeBranch(writer, real_node->right);
        writer->Key(L"left");
        writeBranch(writer, real_node->left);
    }
    break;
    case NodeType::BLOCK: {
        const ASTBlock *real_node = static_cast<const ASTBlock *>(node);
        writer->Key(L"execution");
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
} // namespace LunaScript::compiler::front