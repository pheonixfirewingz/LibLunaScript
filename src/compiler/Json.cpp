#include "Parser.h"
#include <rapidjson/prettywriter.h>
#include <rapidjson/stringbuffer.h>

const char * idToString(const ASTTypeID& id) noexcept
{
    switch (id)
    {
    case AST_ROOT: return "Program";
    case AST_FUNC_DEF: return "FunctionDef";
    case AST_FUNC_CALL: return "FunctionCall";
    case AST_EXPRESSION: return "Expression";
    case AST_BINARY: return "BinaryExpression";
    case AST_LITERAL: return "Literal";
    case AST_BLOCK: return "ExecutionBlock";
    default: return "unknown";
    }
}

const char * idToStringT(const ASTDataType& id) noexcept
{
    switch (id)
    {
    case NOT_DETERMINED_DATA_TYPE: return "To Be Determined";
    case VOID_TYPE: return "Void";
    case INT8_TYPE: return "int8";
    case INT16_TYPE: return "int16";
    case INT32_TYPE: return "int32";
    case INT64_TYPE: return "int64";
    case UINT8_TYPE: return "uint8";
    case UINT16_TYPE: return "uint16";
    case UINT32_TYPE: return "uint32";
    case UINT64_TYPE: return "uint64";
    case FLOAT32_TYPE: return "float32";
    case FLOAT64_TYPE: return "float64";
    case FLOAT_ANY_TYPE: return "any float";
    case UINT_ANY_TYPE: return "any uint";
    case INT_ANY_TYPE: return "any int";
    default: return "unknown";
    }
}

const char * idToStringE(const ASTExpressionType& id) noexcept
{
    switch (id)
    {
    case ASTE_RETURN: return "ReturnType";
    case ASTE_VAR_DEFINED : return "VariableDefinition";
    case ASTE_PRAM_LIST: return "ParameterList";
    default: return "unknown";
    }
}

const char * idToStringO(const ASTOperatorType& id) noexcept
{
    switch (id)
    {
        case ADD_TYPE: return "+";
        case SUB_TYPE: return "-";
        case DIV_TYPE: return "/";
        case MUL_TYPE: return "*";
        case AND_TYPE: return "and";
        case OR_TYPE: return "or";
        case XOR_TYPE: return "xor";
        case MOD_TYPE: return "modulo";
        default: return "unknown";
    }
}
template<typename T>
void writeBranch(T* writer,const ASTNode* node)
{
    if(node == nullptr) 
    {
        writer->StartObject();
        writer->EndObject();
        return;
    }
    writer->StartObject();
    writer->Key("type");
    writer->String(idToString(node->getTypeID()));
    switch(node->getTypeID())
    {
        case AST_ROOT:{
            const ASTRoot* real_node = static_cast<const ASTRoot*>(node);
            writer->Key("body");
            writer->StartArray();
            for(const ASTNode* child: real_node->children)
                writeBranch(writer,child);
            writer->EndArray();
            writer->Key("SourceType");
            writer->String("module");
            break;
        }
        case AST_FUNC_DEF: {
            const ASTFuncDef* real_node = static_cast<const ASTFuncDef*>(node);
            writer->Key("id");
            writer->String(real_node->name.c_str());
            writer->Key("return_type");
            writer->String(idToStringT(real_node->return_type));
            writer->Key("parameters");
            writeBranch(writer,real_node->args);
            writer->Key("body");
            writeBranch(writer,real_node->body);
            break;
        }
        case AST_FUNC_CALL: {
            
        }
        case AST_EXPRESSION: {
            const ASTExpression* real_node = static_cast<const ASTExpression*>(node);
            writer->Key("id");
            writer->String(idToStringE(real_node->type));
            if(!real_node->extra_data.empty())
            {
                writer->Key("name");
                writer->String(real_node->extra_data.c_str());
            }
            writer->Key("declarations");
            writer->StartArray();
            for(const ASTNode* child: real_node->list)
                writeBranch(writer,child);
            writer->EndArray();
            break;
            
        }
        case AST_LITERAL: {
            const ASTLiteral* real_node = static_cast<const ASTLiteral*>(node);
            writer->Key("data_type");
            writer->String(idToStringT(real_node->data_type));
            writer->Key("value");
            writer->String(real_node->value.c_str());
            break;

        }
        case AST_BINARY:{
            const ASTBinaryExpression* real_node = static_cast<const ASTBinaryExpression*>(node);
            writer->Key("presidents");
            writer->Uint(real_node->presidents);
            writer->Key("op");
            writer->String(idToStringO(real_node->op));
            writer->Key("right");
            writeBranch(writer,real_node->right);
            writer->Key("left");
            writeBranch(writer,real_node->left);
            break;
        }
        case AST_BLOCK: {
            const ASTBlock* real_node = static_cast<const ASTBlock*>(node);
            writer->Key("execution");
            writer->StartArray();
            for(const ASTNode* child : real_node->list)
                writeBranch(writer,child);
            writer->EndArray();
        }
        default: break;
    }
    writer->EndObject();
}

const std::string toJson(const ASTRoot* root,const bool pretty_mode) noexcept
{
    rapidjson::StringBuffer s;
    if(pretty_mode)
    {
        rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(s);
        writeBranch<rapidjson::PrettyWriter<rapidjson::StringBuffer>>(&writer,root);
    }
    else
    {
        rapidjson::Writer<rapidjson::StringBuffer> writer(s);
        writeBranch<rapidjson::Writer<rapidjson::StringBuffer>>(&writer,root);
    }
    return s.GetString();
}