#include "ByteCodeGen.h"
#include "IRCodeGen.h"
#include <bitset>

std::string makeHumanReadable(const std::vector<uint64_t> ops) noexcept
{
    std::string ret = "";
    for (size_t i = 0; i < ops.size(); ++i)
    {
        bool is_single = false;
        switch (std::bitset<5>(ops[i]).to_ulong())
        {
        case LunaScript::OP_ADD:
            ret += "add ";
            break;
        case LunaScript::OP_SUB:
            ret += "sub ";
            break;
        case LunaScript::OP_DIV:
            ret += "div ";
            break;
        case LunaScript::OP_MUL:
            ret += "mul ";
            break;
        case LunaScript::OP_JMP:
            ret += "jmp ";
            break;
        case LunaScript::OP_CMP:
            ret += "cmp ";
            break;
        case LunaScript::OP_NCMP:
            ret += "ncmp ";
            break;
        case LunaScript::OP_LOAD:
            ret += "load ";
            break;
        case LunaScript::OP_STORE:
            ret += "store ";
            break;
        case LunaScript::OP_CALL:
            ret += "call ";
            break;
        case LunaScript::OP_RET:
            ret += "ret ";
            break;
        case LunaScript::OP_FADD:
            ret += "addf ";
            break;
        case LunaScript::OP_FSUB:
            ret += "subf ";
            break;
        case LunaScript::OP_FDIV:
            ret += "divf ";
            break;
        case LunaScript::OP_FMUL:
            ret += "mulf ";
            break;
        default:
            printf("DEBUG ERROR: OP NOT IMPLEMENTED %lul \n", std::bitset<5>(ops[i]).to_ulong());
            break;
        }
        ret += "%";
        ret += std::bitset<3>(ops[i] >> 5).to_string();
        if (is_single)
        {
            ret += ", ";
            if (std::bitset<1>(ops[i] >> 8) == true)
            {
                ret += "%";
                ret += std::bitset<3>(ops[i] >> 9).to_string();
            }
            else
            {
                ret += "0x";
                ret += std::bitset<55>(ops[i] >> 9).to_string();
            }
        }
        ret += "\n";
    }
    return ret;
}

std::string makeHumanReadable(const Module* module) noexcept
{
    std::string Str;
    raw_string_ostream OS(Str);
    module->print(OS, nullptr);
    OS.flush();
    return Str;
}