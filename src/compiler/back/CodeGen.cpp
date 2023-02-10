#include "CodeGen.h"
#include <string>

void LunaScript::compiler::back::CodeGenerator::error()
{
    puts("Unimplemented feature\n");
    std::exit(1);
}

std::string LunaScript::compiler::back::CodeGenerator::genFunction(const ASTFuncDef *func)
{
    std::string ret(func->name);
    [[unlikey]] if (func->name == "main")
        ret = "script_main";
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
                ret += genBinary(static_cast<const ASTBinaryExpression*>(data->list[0]));
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

std::string LunaScript::compiler::back::CodeGenerator::genBinary(const ASTBinaryExpression * node)
{
    std::string ret;
    if (isIDaNumeric(node->left->data_type))
    {
        ret += "push const ";
        ret += std::to_string(std::stoul(node->left->value, 0, 16));
        ret += "\n";
        ret += tab_str;
    }
    else
    {
        error();
    }

    if (node->right ->getType() == NodeType::LITERAL)
    {
        if (isIDaNumeric(((const ASTLiteral*)node->right)->data_type))
        {
            ret += "push const ";
            ret += std::to_string(std::stoul(((const ASTLiteral *)node->right)->value, 0, 16));
            ret += "\n";
            ret += tab_str;
        }
        else
        {
            error();
        }
    }
    else
    {
        error();
    }
    ret += "pop r1\n";
    ret += tab_str;
    ret += "pop r2\n";
    ret += tab_str;
    switch (node->op)
    {
    case OperatorType::ADD:
        ret += "add r1 r2\n";
        ret += tab_str;
        break;
    case OperatorType::SUB:
        ret += "sub r1 r2\n";
        ret += tab_str;
        break;
    case OperatorType::DIV:
        ret += "div r1 r2\n";
        ret += tab_str;
        break;
    case OperatorType::MUL:
        ret += "mul r1 r2\n";
        ret += tab_str;
        break;
    case OperatorType::AND:
        ret += "and r1 r2\n";
        ret += tab_str;
        break;
    case OperatorType::OR:
        ret += "or r1 r2\n";
        ret += tab_str;
        break;
    case OperatorType::XOR:
        ret += "xor r1 r2\n";
        ret += tab_str;
        break;
    case OperatorType::MOD:
        ret += "mod r1 r2\n";
        ret += tab_str;
        break;
    }
    ret += "push r1\n";
    ret += tab_str;
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