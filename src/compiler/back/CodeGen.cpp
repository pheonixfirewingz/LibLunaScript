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
    ret += ":\n";
    for (auto &data : func->body->list)
    {
        ret += "  ";
        switch (data->type)
        {
        case ExpressionType::RETURN: {
            if (data->list.size() == 1)
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
                    ret += "\n  ";
                }
            }
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

std::string LunaScript::compiler::back::CodeGenerator::generate()
{
    std::string code;

    for (auto &data : root->children)
    {
        if (data->getType() == NodeType::FUNC_DEF)
        {
            code += genFunction((ASTFuncDef *)data);
            code += "\n";
        }
    }

    return code;
}