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
        return "skip";
    case 0x1F:
        return "no operation/sleep";
    default:
        return "unknown";
    }
}

// TODO: implement floating math
#define OP(code, cast, actor)                                                                                    \
    case OpCode::code: {                                                                                         \
        vm_data_t mem_or_reg;                                                                                    \
        if (op_data.op.is_reg)                                                                                   \
        {                                                                                                        \
            mem_or_reg = getRegister((Register)op_data.op.reg_or_memory_dest);                                   \
            setRegister((Register)op_data.op.reg_or_memory_dest, {(cast)0});                                     \
        }                                                                                                        \
        else                                                                                                     \
            mem_or_reg = getMemory(op_data.op.reg_or_memory_dest);                                               \
        setRegister((Register)op_data.op.reg,                                                                    \
                    {(std::get<cast>(mem_or_reg) actor std::get<cast>(getRegister((Register)op_data.op.reg)))}); \
    }                                                                                                            \
    break

uint64_t LunaScriptVirtualMachine::runNextOp(const uint64_t in) noexcept
{
    OP_DATA op_data(in);
    if (debug_mode)
#if defined(WIN32) || defined(_WIN64)
        printf("\x1B[32mOP:\x1B[37m %s, \x1B[32mREG:\x1B[33m %llu, \x1B[32mis actor register:\x1B[33m %s,\x1B[32mis "
               "actor a const value:\x1B[33m %s, \x1B[32mLOC:\x1B[33m %llu\033[0m\t\t\n",
               opToStr(op_data.op.op), op_data.op.reg, op_data.op.is_reg == 1 ? "true" : "false",
               op_data.op.is_constant == 1 ? "true" : "false", op_data.op.reg_or_memory_dest);
#else
        printf("\x1B[32mOP:\x1B[37m %s, \x1B[32mREG:\x1B[33m %u, \x1B[32mis actor register:\x1B[33m %s,\x1B[32mis "
               "actor a const value:\x1B[33m %s, \x1B[32mLOC:\x1B[33m %lu\033[0m\t\t\n",
               opToStr(op_data.op.op), op_data.op.reg, op_data.op.is_reg == 1 ? "true" : "false",
               op_data.op.is_constant == 1 ? "true" : "false", op_data.op.reg_or_memory_dest);
#endif
    switch ((OpCode)op_data.op.op)
    {
        OP(ADD, uint64_t, +);
        OP(SUB, uint64_t, -);
        OP(DIV, uint64_t, /);
        OP(MUL, uint64_t, *);
    case OpCode::JMP: {
        return_stack->push(pic + 1);
        set_pic = true;
        return op_data.op.reg_or_memory_dest;
    }
    break;
    case OpCode::SKIP: {
        set_pic = true;
        return op_data.op.reg_or_memory_dest;
    }
    break;
    case OpCode::CMP: {
        vm_data_t mem_or_reg;
        if (op_data.op.is_reg)
            mem_or_reg = getRegister((Register)op_data.op.reg_or_memory_dest);
        else
            mem_or_reg = getMemory(op_data.op.reg_or_memory_dest);
        if (std::get<uint64_t>(mem_or_reg) == std::get<uint64_t>(getRegister((Register)op_data.op.reg)))
        {
            set_pic = true;
            return pic + 2;
        }
    }
    break;
    case OpCode::NCMP: {
        vm_data_t mem_or_reg;
        if (op_data.op.is_reg)
            mem_or_reg = getRegister((Register)op_data.op.reg_or_memory_dest);
        else
            mem_or_reg = getMemory(op_data.op.reg_or_memory_dest);
        if (std::get<uint64_t>(mem_or_reg) != std::get<uint64_t>(getRegister((Register)op_data.op.reg)))
        {
            set_pic = true;
            return pic + 2;
        }
    }
    break;
    case OpCode::PUSH: {
        vm_data_t mem_or_reg;
        if (op_data.op.is_reg)
        {
            mem_or_reg = getRegister((Register)op_data.op.reg_or_memory_dest);
            setRegister((Register)op_data.op.reg_or_memory_dest, 0);
        }
        else if (op_data.op.is_constant)
            mem_or_reg = (uint64_t)op_data.op.reg_or_memory_dest;
        else
            mem_or_reg = getMemory(op_data.op.reg_or_memory_dest);
        push_(mem_or_reg);
    }
    break;
    case OpCode::POP: {
        vm_data_t value = pop();
        setRegister((Register)op_data.op.reg, value);
        break;
    }
    case OpCode::STORE:
        data->vmErrorCallback("STORE not implemented");
        break;
    case OpCode::LOAD:
        data->vmErrorCallback("LOAD not implemented");
        break;
    case OpCode::CALL: {
        vm_data_t *data = execute(op_data.op.reg_or_memory_dest, stack);
        if (data)
            push_(*data);
    }
    break;
    case OpCode::RET: {
        set_pic = true;
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
        vm_data_t mem_or_reg;
        if (op_data.op.is_reg)
        {
            mem_or_reg = getRegister((Register)op_data.op.reg_or_memory_dest);
            setRegister((Register)op_data.op.reg_or_memory_dest, 0);
        }
        else if (op_data.op.is_constant)
            mem_or_reg = (uint64_t)op_data.op.reg_or_memory_dest;
        else
            mem_or_reg = getMemory(op_data.op.reg_or_memory_dest);
        setRegister((Register)op_data.op.reg, mem_or_reg);
    }
    break;
    case OpCode::NOP:
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(1ms);
        break;
    default:
        data->vmErrorCallback("operator unknown");
        break;
    }
    return 0;
}
} // namespace LunaScript::lsvm