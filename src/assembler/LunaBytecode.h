#pragma once
#include <stdint.h>

namespace LunaScript::bytecode
{
// max instructions 31. or 0-31
enum class OpCode : uint8_t
{
    ADD = 0x00,   // 31
    SUB = 0x01,   // 30
    DIV = 0x02,   // 29
    MUL = 0x03,   // 28
    JMP = 0x04,   // 27
    CMP = 0x05,   // 26
    NCMP = 0x06,  // 25
    PUSH = 0x07,  // 24
    POP = 0x08,   // 23
    STORE = 0x09, // 22
    LOAD = 0x0A,  // 21
    CALL = 0x0B,  // 20
    RET = 0x0C,   // 19
    MOV = 0x0D,   // 18
    FADD = 0x0E,  // 17
    FSUB = 0x0F,  // 16
    FDIV = 0x10,  // 15
    FMUL = 0x11,  // 14
    ICALL = 0x12, // 13
    // can add
    NOP = 0x1F // 0
};

enum class Register : uint64_t
{
    NONE = 0x00,
    R1 = 0x01,
    R2 = 0x02,
    R3 = 0x03,
    R4 = 0x04,
    R5 = 0x05,
    R6 = 0x06,
    R7 = 0x07,
};

struct ByteCode
{
    uint64_t op : 5;
    uint64_t reg : 3;
    uint64_t is_reg : 1;
    uint64_t is_constant : 1;
    uint64_t reg_or_memory_dest : 53;
    uint64_t reserved_0 : 1 = 0;

    uint64_t get() noexcept
    {
        return op | reg << 5 | is_reg << 8 | is_constant << 9 | reg_or_memory_dest << 10 | (uint64_t)reserved_0 << 63;
    }

    void clear() noexcept
    {
        op = (uint64_t)OpCode::NOP;
        reg = (uint64_t)Register::NONE;
        is_reg = false;
        is_constant = false;
        reg_or_memory_dest = 0;
        reserved_0 = 0;
    }

    void setOP(OpCode op_in) noexcept
    {
        op = (uint64_t)op_in;
    }

    void setConst() noexcept
    {
        is_constant = true;
    }

    void setReg(Register reg_in, bool is_first) noexcept
    {
        if (is_first)
            reg_or_memory_dest = (uint64_t)reg_in;
        else
        {
            is_reg = (uint64_t) true;
            reg = (uint64_t)reg_in;
        }
    }

    void regToMemory()
    {
        reg_or_memory_dest = reg;
        reg = 0;
    }

    template<typename T> void setMemory(T reg_or_mem_dest)
    {
        is_reg = (uint64_t) false;
        reg_or_memory_dest = (uint64_t)reg_or_mem_dest;
    }
};

}; // namespace LunaScript::bytecode