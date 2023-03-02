#include "CodeGen.h"
#include <algorithm>
#include <string>

void LunaScript::compiler::back::CodeGenerator::error()
{
    printf("ASM GENERATED:\n%s\n\n%s\n", ret.c_str(), "Unimplemented feature");
    std::exit(1);
}

void LunaScript::compiler::back::CodeGenerator::genFunction(const ASTFuncDef *func)
{
    ret += func->name;
    [[unlikely]] if (func->name == "main") ret = "script_main";
    ret += ":\n";
    for (auto &data : func->body->list)
    {
        switch (data->type)
        {
        case ExpressionType::VAR_DEFINED: {
            if (data->list[0]->getType() == NodeType::BINARY)
                genBinary(static_cast<const ASTBinaryExpression *>(data->list[0]));
            else
            {
                ret += genLiteral(0, static_cast<const ASTLiteral *>(data->list[0]));
                ret += "push r1";
            }
        }
            continue;
        case ExpressionType::RETURN: {
            if (data->list[0]->getType() == NodeType::LITERAL)
            {
                const ASTLiteral *value = (ASTLiteral *)data->list[0];
                if (value->data_type == DataType::NOT_DETERMINED)
                {
                    error();
                }
                else
                {
                    ret += "push const ";
                    ret += std::to_string(std::stoul(value->value, 0, 16));
                    ret += "\n";
                }
            }
            else if (data->list[0]->getType() == NodeType::BINARY)
                genBinary(static_cast<const ASTBinaryExpression *>(data->list[0]));
        }
            [[fallthrough]];
        case ExpressionType::NO_RETURN: {
            ret += "ret\n";
            break;
        }
        default: {
            error();
        }
        break;
        }
    }
}

void LunaScript::compiler::back::CodeGenerator::genBinary(const ASTBinaryExpression *node)
{
    std::vector<const BinaryInfo *> infos;
    const ASTBinaryExpression *current = node;
    for (; current->right->getType() == NodeType::BINARY;)
    {
        infos.push_back(new BinaryInfo(current->precedence, current->op, current->left, nullptr));
        current = static_cast<const ASTBinaryExpression *>(current->right);
    }
    infos.push_back(new BinaryInfo(current->precedence, current->op, current->left,
                                   static_cast<const ASTLiteral *>(current->right)));
    std::sort(infos.begin(), infos.end(),
              [](const BinaryInfo *lhs, const BinaryInfo *rhs) { return lhs->precedence > rhs->precedence; });

    std::string block;
    std::string data;
    for (auto &info : infos)
    {
        if (info->right != nullptr)
        {
            data += genLiteral(1, info->left);
            block += genLiteral(2, info->right);
        }
        else
        {
            block += genLiteral(2, info->left);
        }
        switch (info->op)
        {
        case OperatorType::ADD:
            block += "add r1 r2\n";
            break;
        case OperatorType::SUB:
            block += "sub r1 r2\n";
            break;
        case OperatorType::DIV:
            block += "div r1 r2\n";
            break;
        case OperatorType::MUL:
            block += "mul r1 r2\n";
            break;
        case OperatorType::AND:
            block += "and r1 r2\n";
            break;
        case OperatorType::OR:
            block += "or r1 r2\n";
            break;
        case OperatorType::XOR:
            block += "xor r1 r2\n";
            break;
        case OperatorType::MOD:
            block += "mod r1 r2\n";
            break;
        case OperatorType::NOT_DETERMINED:
            break;
        }
    }
    ret += data;
    ret += block;
    ret += "push r1\n";
}

std::string LunaScript::compiler::back::CodeGenerator::genLiteral(const uint8_t reg, const ASTLiteral *node,
                                                                  bool global)
{
    std::string data;
    if (global)
    {
        if (isIDaNumeric(node->data_type))
        {
            data += "store ";
            data += "const ";
            data += node->value;
            data += "\n";
        }
        else if (!isIDaNumeric(node->data_type))
        {
        }
        else
        {
            error();
        }
    }
    else
    {
        if (isIDaNumeric(node->data_type))
        {
            data += "mov r";
            data += std::to_string(reg);
            data += " const ";
            data += node->value;
            data += "\n";
        }
        else if (!isIDaNumeric(node->data_type))
        {
        }
        else
        {
            error();
        }
    }
    return data;
}

std::string LunaScript::compiler::back::CodeGenerator::generate()
{
    for (auto &data : root->children)
    {
        if (data->getType() == NodeType::FUNC_DEF)
            genFunction((ASTFuncDef *)data);
    }

    return ret;
}