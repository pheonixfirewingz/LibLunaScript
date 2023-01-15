#include "VirtualMachine.h"
namespace LunaScript::lsvm
{
static const char *opToStr(const uint64_t op) noexcept
{
    switch (op)
    {
    case 0x00:
        return "add";
    case 0x01:
        return "subtract";
    case 0x02:
        return "divide";
    case 0x03:
        return "multiply";
    case 0x04:
        return "jump";
    case 0x05:
        return "compare";
    case 0x06:
        return "not compare";
    case 0x07:
        return "push";
    case 0x08:
        return "pop";
    case 0x09:
        return "store";
    case 0x0A:
        return "load";
    case 0x0B:
        return "call";
    case 0x0C:
        return "return";
    case 0x0D:
        return "move";
    case 0x0E:
        return "float add";
    case 0x0F:
        return "float subtract";
    case 0x10:
        return "float divide";
    case 0x11:
        return "float multiply";
    case 0x12:
        return "internal call";
    case 0x1F:
        return "no operation";
    default:
        return "unknown";
    }
}

#define OP(code, type, cast, actor)                                                              \
    case OpCode::code: {                                                                         \
        VM_DATA mem_or_reg;                                                                      \
        if (op_data.op.is_reg)                                                                   \
        {                                                                                        \
            mem_or_reg = getRegister((Register)op_data.op.reg_or_memory_dest);                   \
            setRegister((Register)op_data.op.reg_or_memory_dest, {(cast)0});                     \
        }                                                                                        \
        else                                                                                     \
            mem_or_reg = getMemory(op_data.op.reg_or_memory_dest);                               \
        setRegister((Register)op_data.op.reg,                                                    \
                    {(cast)(mem_or_reg.type actor getRegister((Register)op_data.op.reg).type)}); \
    }                                                                                            \
    break

uint64_t LunaScriptVirtualMachine::runNextOp(const uint64_t in) noexcept
{
    OP_DATA op_data(in);
    printf("OP:%s, REG:%u, is actor register:%s,is actor a const value:%s, LOC:%lu\n", opToStr(op_data.op.op),
           op_data.op.reg, op_data.op.is_reg == 1 ? "true" : "false", op_data.op.is_constant == 1 ? "true" : "false",
           op_data.op.reg_or_memory_dest);
    switch ((OpCode)op_data.op.op)
    {
        OP(ADD, integer_, uint64_t, +);
        OP(SUB, integer_, uint64_t, -);
        OP(DIV, integer_, uint64_t, /);
        OP(MUL, integer_, uint64_t, *);
    case OpCode::JMP:{
        return_stack->push(pic + 1);
        return op_data.op.reg_or_memory_dest;
    }
    break;
    case OpCode::CMP: {
        VM_DATA mem_or_reg;
        if (op_data.op.is_reg)
            mem_or_reg = getRegister((Register)op_data.op.reg_or_memory_dest);
        else
            mem_or_reg = getMemory(op_data.op.reg_or_memory_dest);
        setRegister((Register)op_data.op.reg,
                    {(uint64_t)(mem_or_reg.integer_ == getRegister((Register)op_data.op.reg).integer_)});
    }
    break;
    case OpCode::NCMP: {
        VM_DATA mem_or_reg;
        if (op_data.op.is_reg)
            mem_or_reg = getRegister((Register)op_data.op.reg_or_memory_dest);
        else
            mem_or_reg = getMemory(op_data.op.reg_or_memory_dest);
        setRegister((Register)op_data.op.reg,
                    {(uint64_t)(mem_or_reg.integer_ != getRegister((Register)op_data.op.reg).integer_)});
    }
    break;
    case OpCode::PUSH: {
        VM_DATA mem_or_reg;
        if (op_data.op.is_reg)
        {
            mem_or_reg = getRegister((Register)op_data.op.reg_or_memory_dest);
            setRegister((Register)op_data.op.reg_or_memory_dest, {(uint64_t)0});
        }
        else if (op_data.op.is_constant)
            mem_or_reg = {op_data.op.reg_or_memory_dest};
        else
            mem_or_reg = getMemory(op_data.op.reg_or_memory_dest);
        push(mem_or_reg);
    }
    break;
    case OpCode::POP: {
        VM_DATA value = pop();
        setRegister((Register)op_data.op.reg, value);
    }
    case OpCode::STORE:
        data->vmErrorCallback("STORE not implemented");
        break;
    case OpCode::LOAD:
        data->vmErrorCallback("LOAD not implemented");
        break;
    case OpCode::CALL:
        data->vmErrorCallback("CALL not implemented");
        break;
    case OpCode::RET: {
        if (!return_stack->empty())
        {
            uint64_t result = return_stack->top();
            return_stack->pop();
            return result;
        }
        else
            return UINT64_MAX;
    }
    break;
    case OpCode::MOV: {
        VM_DATA mem_or_reg;
        if (op_data.op.is_reg)
        {
            mem_or_reg = getRegister((Register)op_data.op.reg_or_memory_dest);
            setRegister((Register)op_data.op.reg_or_memory_dest, {(uint64_t)0});
        }
        else if (op_data.op.is_constant)
            mem_or_reg = {op_data.op.reg_or_memory_dest};
        else
            mem_or_reg = getMemory(op_data.op.reg_or_memory_dest);
        setRegister((Register)op_data.op.reg, mem_or_reg);
    }
    break;
        OP(FADD, float_, float64_t, +);
        OP(FSUB, float_, float64_t, -);
        OP(FDIV, float_, float64_t, /);
        OP(FMUL, float_, float64_t, *);
    case OpCode::ICALL:
        data->vmErrorCallback("ICALL not implemented");
        break;
    case OpCode::NOP:
        break;
    default:
        data->vmErrorCallback("operator unknown");
        break;
    }
    return 0;
}
} // namespace LunaScript::lsvm