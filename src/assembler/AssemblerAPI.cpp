#include "Lexer.h"
#include "LunaBytecode.h"
#include <functional>
#include <liblunascript/Assembler.h>
#include <map>
#include <string>

#define REG(x)                                                       \
    case LunaScript::assembler::LexerToken::x: {                     \
        byte_code.setReg(LunaScript::bytecode::Register::x, second); \
        second = true;                                               \
    }                                                                \
    break

#define OP(x)                                             \
    case LunaScript::assembler::LexerToken::x: {          \
        byte_code.setOP(LunaScript::bytecode::OpCode::x); \
        op_mode = true;                                   \
    }                                                     \
    break

#define SPECIAL_OP(x, mode)                               \
    case LunaScript::assembler::LexerToken::x: {          \
        byte_code.setOP(LunaScript::bytecode::OpCode::x); \
        op_mode = true;                                   \
        mode = true;                                      \
    }                                                     \
    break

std::ReadOnlyVector<uint64_t> assemble(const char *src, const data_size_t src_size)
{
    LunaScript::assembler::Lexer lexer(std::string(src, 0, src_size));
    std::ReadOnlyVector<LunaScript::assembler::LexToken> tokens = lexer.retrieve();
    std::map<uint64_t, uint64_t> label_memory_map;
    uint64_t current_op_code = 0;
    (void)tokens;
    std::vector<uint64_t> ops;
    LunaScript::bytecode::ByteCode byte_code;
    bool second = false;
    bool op_mode = false;
    bool call_mode = false;
    bool push_mode = false;
    bool has_op = false;
    std::string last_identifier;
    for (auto &token : tokens)
    {
        switch (token.token)
        {
        case LunaScript::assembler::LexerToken::LABEL: {
            label_memory_map.emplace(std::hash<std::string>{}(last_identifier), current_op_code);
            if (last_identifier == "script_main")
                current_op_code++;
        }
        break;
        case LunaScript::assembler::LexerToken::IDENTIFIER:
            last_identifier = token.str_token;
            break;
        case LunaScript::assembler::LexerToken::NEW_LINE: {
            if (has_op)
                current_op_code++;
            has_op = false;
        }
        break;
        default:
            if (!has_op)
                has_op = true;
            break;
        }
    }
    for (auto &token : tokens)
    {
        switch (token.token)
        {
            REG(R1);
            REG(R2);
            REG(R3);
            REG(R4);
            REG(R5);
            REG(R6);
            REG(R7);
            OP(ADD);
            OP(SUB);
            OP(DIV);
            OP(MUL);
            SPECIAL_OP(JMP, call_mode);
            OP(CMP);
            OP(NCMP);
            SPECIAL_OP(PUSH, push_mode);
            OP(POP);
            OP(STORE);
            OP(LOAD);
            OP(RET);
            OP(MOV);
            OP(FADD);
            OP(FSUB);
            OP(FDIV);
            OP(FMUL);
        case LunaScript::assembler::LexerToken::CONST: {
            byte_code.setConst();
        }
        break;
            SPECIAL_OP(CALL, call_mode);
        case LunaScript::assembler::LexerToken::LABEL: {
            if (last_identifier == "script_main")
                ops.push_back(UINT64_MAX);
        }
        break;
        case LunaScript::assembler::LexerToken::IDENTIFIER: {
            if (op_mode && !call_mode)
                byte_code.setMemory<uint64_t>(std::stoull(token.str_token, nullptr, 16));
            else if (call_mode)
            {
                if (auto search = label_memory_map.find(std::hash<std::string>{}(token.str_token));
                    search != label_memory_map.end())
                    byte_code.setMemory<uint64_t>(search->second);
                else
                    byte_code.setMemory<uint64_t>(std::hash<std::string>{}(token.str_token));
            }
            else
                last_identifier = token.str_token;
        }
        break;
        case LunaScript::assembler::LexerToken::NEW_LINE: {
            if (push_mode)
            {
                if (byte_code.is_reg)
                    byte_code.regToMemory();
            }
            if (op_mode)
                ops.push_back(byte_code.get());
            byte_code.clear();
            second = false;
            op_mode = false;
            call_mode = false;
            push_mode = false;
        }
        break;
        default:
            break;
        }
    }
    return std::ReadOnlyVector<uint64_t>::lock(ops);
}