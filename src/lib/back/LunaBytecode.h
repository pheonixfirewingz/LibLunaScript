#pragma once
#include <stdint.h>

namespace LunaScript
{
// max instructions 31. or 0-31
typedef enum LunaScriptByteCode : uint8_t
{
    OP_ADD = 0x00,  // 31
    OP_SUB = 0x01,  // 30
    OP_DIV = 0x02,  // 29
    OP_MUL = 0x03,  // 28
    OP_JMP = 0x04,  // 27
    OP_CMP = 0x05,  // 26
    OP_NCMP = 0x06, // 25
    OP_PUSH = 0x07, // 24
    OP_POP = 0x08,  // 23
    OP_CALL = 0x09, // 22
    OP_RET = 0x10,  // 21
    OP_FADD = 0x11, // 20
    OP_FSUB = 0x12, // 19
    OP_FDIV = 0x13, // 18
    OP_FMUL = 0x14, // 17
} LunaScriptByteCode;

typedef struct ByteCodeOperation
{
    uint64_t op : 5;
    uint64_t reg : 3;
    uint64_t is_reg : 1;
    uint64_t reg_or_memory_dest : 55;
} ByteCodeOperation;
}; // namespace LunaScript