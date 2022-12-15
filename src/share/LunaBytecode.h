#pragma once
#include <stdint.h>

namespace LunaScript
{
// max instructions 31. or 0-31
typedef enum LunaScriptByteCode : uint8_t
{
    OP_ADD = 0x00,   // 31
    OP_SUB = 0x01,   // 30
    OP_DIV = 0x02,   // 29
    OP_MUL = 0x03,   // 28
    OP_JMP = 0x04,   // 27
    OP_CMP = 0x05,   // 26
    OP_NCMP = 0x06,  // 25
    OP_LOAD = 0x07,  // 24
    OP_STORE = 0x08, // 23
    OP_CALL = 0x09,  // 22
} LunaScriptByteCode;

typedef struct Function
{
    uint64_t function_name_hash;
    uint64_t bytecode_index_offset;
} Function;

typedef struct ByteCodeOperation
{
    uint64_t op: 5;
    uint64_t reg : 3;
    uint64_t is_reg: 1;
    uint64_t reg_or_memory_dest : 55;
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