#include "ByteCodeGen.h"
#include <tuple>
#include <bitset>

struct functionIndex
{
    const char *name;
    const uint64_t index;
    constexpr functionIndex(const char *name, const uint64_t index)
        : name(name)
        , index(index)
    {
    }

    const char *getName() const noexcept
    {
        return name;
    }

    const uint64_t *getIndex() const noexcept
    {
        return &index;
    }
};

std::vector<uint64_t> generateBlock(const ASTBlock *node, const std::vector<functionIndex> &)
{
    std::vector<uint64_t> block;
    for (const auto &node : node->list)
    {
        switch (node->getTypeID())
        {
        case AST_EXPRESSION: {
            const auto &expression = dynamic_cast<const ASTExpression *>(node);
            switch (expression->type)
            {
                case ASTE_RETURN:
                {
                    //temp
                    block.push_back(LunaScript::OP_RET);
                }
                case ASTE_VAR_DEFINED:
            default:
                break;
            }
        }
        case AST_FUNC_CALL:
        default:
            break;
        }
    }
    return block;
}

const std::vector<uint64_t> generateByteCode(const ASTRoot *root) noexcept
{

    std::vector<uint64_t> byte_code;
    std::vector<functionIndex> function_index_lookup;
    for (auto &child : root->children)
    {
        switch (child->getTypeID())
        {
        case AST_FUNC_DEF: {
            auto func = dynamic_cast<const ASTFuncDef *>(child);
            function_index_lookup.push_back({func->name.c_str(), byte_code.size()});
            for(auto& bc:generateBlock(func->body,function_index_lookup))
                byte_code.push_back(bc);
        }
        case AST_EXPRESSION:
            break;
        default:
            break;
        }
    }
    return byte_code;
}