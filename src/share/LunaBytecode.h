#pragma once
#include <stdint.h>

namespace LunaScript
{
// max instructions 63. or 0-62
typedef enum LunaScriptByteCode : uint8_t
{
    OP_ADD = 0x00,   // 62
    OP_SUB = 0x01,   // 61
    OP_DIV = 0x02,   // 60
    OP_MUL = 0x03,   // 59
    OP_JMP = 0x04,   // 58
    OP_CMP = 0x05,   // 57
    OP_NCMP = 0x06,  // 56
    OP_LOAD = 0x07,  // 55
    OP_STORE = 0x08, // 54
    OP_CALL = 0x09,  // 53
} LunaScriptByteCode;

typedef struct Function
{
    uint64_t function_name_hash;
    uint64_t bytecode_index_offset;
} Function;

typedef struct ByteCodeOperation
{
    uint8_t op;
    uint32_t reg_or_memory_src : 28;
    uint32_t reg_or_memory_dest : 28;
} ByteCodeOperation;

typedef struct Program
{
    // functions index length
    uint64_t functions_index_length;
    // functions ByteCode length
    uint64_t functions_ByteCode_length;
    // functions index;
    Function *functions_index;
    // byte code
    ByteCodeOperation *byteCode_operations;
} Program;
}; // namespace LunaScript