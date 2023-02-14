#include "CodeGen.h"
#include <algorithm>
#include <string>

void LunaScript::compiler::back::CodeGenerator::error()
{
    puts("Unimplemented feature\n");
    std::exit(1);
}

std::string LunaScript::compiler::back::CodeGenerator::genFunction(const ASTFuncDef *func)
{
    std::string ret(func->name);
    [[unlikely]] if (func->name == "main") ret = "script_main";
    ret += ":\n";
    for (auto &data : func->body->list)
    {
        ret += tab_str;
        switch (data->type)
        {
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
                    ret += tab_str;
                }
            }
            else if (data->list[0]->getType() == NodeType::BINARY)
                ret += genBinary(static_cast<const ASTBinaryExpression *>(data->list[0]));
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
    return ret;
}

std::string LunaScript::compiler::back::CodeGenerator::genBinary(const ASTBinaryExpression *node)
{
    std::string ret;
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
        if(info->right != nullptr)
        {
            data += genLiteral(1,info->left);
            data += tab_str;
            block += genLiteral(2,info->right);
            block += tab_str;
        }
        else
        {
            block += genLiteral(2,info->left);
            block += tab_str;
        }
        switch (info->op)
        {
        case OperatorType::ADD:
            block += "add r1 r2\n";
            block += tab_str;
            break;
        case OperatorType::SUB:
            block += "sub r1 r2\n";
            block += tab_str;
            break;
        case OperatorType::DIV:
            block += "div r1 r2\n";
            block += tab_str;
            break;
        case OperatorType::MUL:
            block += "mul r1 r2\n";
            block += tab_str;
            break;
        case OperatorType::AND:
            block += "and r1 r2\n";
            block += tab_str;
            break;
        case OperatorType::OR:
            block += "or r1 r2\n";
            block += tab_str;
            break;
        case OperatorType::XOR:
            block += "xor r1 r2\n";
            block += tab_str;
            break;
        case OperatorType::MOD:
            block += "mod r1 r2\n";
            block += tab_str;
            break;
        case OperatorType::NOT_DETERMINED:
            break;
        }
    }
    ret += data;
    ret += block;
    ret += "push r1\n";
    ret += tab_str;
    return ret;
}

std::string LunaScript::compiler::back::CodeGenerator::genLiteral(const uint8_t reg,const ASTLiteral *node)
{
    std::string ret;
    if (isIDaNumeric(node->data_type))
    {
        ret += "mov r";
        ret += std::to_string(reg);
        ret += " const ";
        ret += node->value;
        ret += "\n";
    }
    else
    {
        error();
    }
    return ret;
}

std::string LunaScript::compiler::back::CodeGenerator::generate()
{
    std::string code;

    for (auto &data : root->children)
    {
        if (data->getType() == NodeType::FUNC_DEF)
            code += genFunction((ASTFuncDef *)data);
    }

    return code;
}