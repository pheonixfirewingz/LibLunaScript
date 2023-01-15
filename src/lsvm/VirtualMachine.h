#pragma once
#include "../assembler/LunaBytecode.h"
#include <initializer_list>
#include <liblunascript/Lsvm.h>
#include <stack>
#include <stdint.h>

namespace LunaScript::lsvm
{
using namespace LunaScript::bytecode;
class LunaScriptVirtualMachine
{
    const VMData *data;
    union VM_DATA {
        uint64_t integer_;
        float64_t float_;
        VM_DATA() = default;
        VM_DATA(const float64_t data)
            : float_(data){};
        VM_DATA(const uint64_t data)
            : integer_(data){};
    };
    std::stack<VM_DATA> *stack;
    std::stack<uint64_t> *return_stack;
    VM_DATA r1 = {(uint64_t)0};
    VM_DATA r2 = {(uint64_t)0};
    VM_DATA r3 = {(uint64_t)0};
    VM_DATA r4 = {(uint64_t)0};
    VM_DATA r5 = {(uint64_t)0};
    VM_DATA r6 = {(uint64_t)0};
    VM_DATA r7 = {(uint64_t)0};
    uint64_t pic = 0;

    union OP_DATA {
        uint64_t value;
        ByteCode op;
        explicit OP_DATA(uint64_t value)
            : value(value)
        {
        }
    };

    void push(VM_DATA value) noexcept
    {
        stack->push(value);
    }

    VM_DATA pop() noexcept
    {
        VM_DATA value = stack->top();
        stack->pop();
        return value;
    }

    VM_DATA getRegister(const Register reg) const noexcept
    {
        switch (reg)
        {
        case Register::R1:
            return r1;
        case Register::R2:
            return r2;
        case Register::R3:
            return r3;
        case Register::R4:
            return r4;
        case Register::R5:
            return r5;
        case Register::R6:
            return r6;
        case Register::R7:
            return r7;
        case Register::NONE:
            data->vmErrorCallback("tried to access register that does not exist");
            return {(uint64_t)0};
        }
    }

    void setRegister(const Register reg, const VM_DATA value) noexcept
    {
        switch (reg)
        {
        case Register::R1:
            r1 = value;
            break;
        case Register::R2:
            r2 = value;
            break;
        case Register::R3:
            r3 = value;
            break;
        case Register::R4:
            r4 = value;
            break;
        case Register::R5:
            r5 = value;
            break;
        case Register::R6:
            r6 = value;
            break;
        case Register::R7:
            r1 = value;
            break;
        case Register::NONE:
            data->vmErrorCallback("tried to access register that does not exist");
            break;
        }
    }

    VM_DATA getMemory(uint64_t) const noexcept
    {
        data->vmErrorCallback("memory is not implemented");
        return {(uint64_t)0};
    }

    uint64_t runNextOp(const uint64_t op) noexcept;

  public:
    explicit LunaScriptVirtualMachine(const VMData *data_in,const std::ReadOnlyVector<uint64_t> ops)
        : data(data_in)
        , stack(new std::stack<VM_DATA>())
        , return_stack(new std::stack<uint64_t>())
    {
        for (; pic <= ops.size();)
        {
            uint64_t new_pic;
            if ((new_pic = runNextOp(ops[pic])) != 0)
                pic = new_pic;
            else 
                pic++;
            if (new_pic == UINT64_MAX)
                break;
        }
    }

    ~LunaScriptVirtualMachine()
    {
        data->vmExitCallback(pop().integer_);
    }
};
} // namespace LunaScript::lsvm