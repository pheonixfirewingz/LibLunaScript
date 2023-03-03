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
    [[unlikely]] if (func->name == "main")
        ret = "script_main";
    ret += ":\n";

    for (auto *expr : func->body->list)
    {
        switch (expr->getType())
        {
        case NodeType::EXPRESSION:
            genExpression(expr);
            break;
        default:
            error();
        }
    }
    tracker.wipe();
}

void LunaScript::compiler::back::CodeGenerator::genExpression(const ASTExpression *expr)
{
    switch (expr->getExprType())
    {
    case ExpressionType::RETURN: {
        const auto *cast = static_cast<const ASTReturnExpression *>(expr);
        genExpression(cast->child);
        ret += "push ";
        ret += tracker.getReg(tracker.getLastUsedReg());
        ret += "\nret\n";
    }
    break;
    case ExpressionType::NO_RETURN: {
    }
    break;
    case ExpressionType::VAR_DEFINED: {
        const auto *cast = static_cast<const ASTLiteral *>(expr);
        if (cast->global)
            global_map.insert({std::hash<std::string>{}(std::string(cast->name.c_str())), cast->value});
        else
        {
            if (cast->hasChild())
                genExpression(cast->child);
            else
            {
                tracker.setFreeReg(cast->name);
                ret += "mov ";
                ret += tracker.getReg(cast->name);
                ret += " const ";
                ret += cast->value;
                ret += "\n";
            }
        }
    }
    break;
    case ExpressionType::PRAM_LIST: {
    }
    break;
    case ExpressionType::FUNC_CALL: {
    }
    break;
    case ExpressionType::BINARY: {
        std::vector<const BinaryInfo *> infos;
        const ASTExpression *current = expr;
        for (; current->getExprType() == ExpressionType::BINARY;)
        {
            const ASTBinaryExpression *cast = static_cast<const ASTBinaryExpression *>(current);
            infos.push_back(new BinaryInfo(cast->precedence, cast->op, cast->left, nullptr));
            current = static_cast<const ASTExpression *>(cast->right);
        }
        infos.push_back(new BinaryInfo(static_cast<const ASTBinaryExpression *>(current)->precedence,
                                       static_cast<const ASTBinaryExpression *>(current)->op,
                                       static_cast<const ASTBinaryExpression *>(current)->left,
                                       static_cast<const ASTLiteral *>(static_cast<const ASTBinaryExpression *>(current)->right)));
        std::sort(infos.begin(), infos.end(),
                  [](const BinaryInfo *lhs, const BinaryInfo *rhs) { return lhs->precedence > rhs->precedence; });
    }
    break;
    case ExpressionType::LITERAL: {
        const auto *cast = static_cast<const ASTLiteral *>(expr);
        tracker.setFreeReg(cast->name);
        ret += "mov ";
        ret += tracker.getReg(cast->name);
        ret += " const ";
        ret += cast->value;
        ret += "\n";
    }
    break;
    default:
        error();
    }
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