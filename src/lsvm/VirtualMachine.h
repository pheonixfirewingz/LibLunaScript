#pragma once
#include "../assembler/LunaBytecode.h"
#include <liblunascript/Lsvm.h>
#include <stack>
#include <stdint.h>

namespace LunaScript::lsvm
{
using namespace LunaScript::bytecode;
class LunaScriptVirtualMachine
{
    const VMData *data;
    std::stack<vm_data_t> *stack;
    std::stack<uint64_t> *return_stack;
    vm_data_t r1 = {(uint64_t)0};
    vm_data_t r2 = {(uint64_t)0};
    vm_data_t r3 = {(uint64_t)0};
    vm_data_t r4 = {(uint64_t)0};
    vm_data_t r5 = {(uint64_t)0};
    vm_data_t r6 = {(uint64_t)0};
    vm_data_t r7 = {(uint64_t)0};
    uint64_t pic = 0;
    bool stop = false;
    bool set_pic = false;
    const bool debug_mode;

    union OP_DATA {
        uint64_t value;
        ByteCode op;
        explicit OP_DATA(uint64_t value)
            : value(value)
        {
        }
    };

    void push(vm_data_t value) noexcept
    {
        stack->push(value);
    }

    vm_data_t pop() noexcept
    {
        vm_data_t value = stack->top();
        stack->pop();
        return value;
    }

    vm_data_t getRegister(const Register reg) const noexcept
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

    void setRegister(const Register reg, const vm_data_t value) noexcept
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

    vm_data_t getMemory(uint64_t) const noexcept
    {
        data->vmErrorCallback("memory is not implemented");
        return {(uint64_t)0};
    }

    uint64_t runNextOp(const uint64_t op) noexcept;

    vm_data_t *execute(const uint64_t name, std::stack<vm_data_t> *stack)
    {
        auto func = data->vmCallbacks.find(name);
        if (func)
            return (*func)(stack);
        else
            data->vmErrorCallback(
                (std::string("tried to execute a non registered function (Hash Name): ") += std::to_string(name))
                    .c_str());
        stop = true;
        return nullptr;
    }

  public:
    explicit LunaScriptVirtualMachine(const VMData *data_in, const std::ReadOnlyVector<uint64_t> ops,
                                      bool debug_mode_in)
        : data(data_in)
        , stack(new std::stack<vm_data_t>())
        , return_stack(new std::stack<uint64_t>())
        , debug_mode(debug_mode_in)
    {
        pic = ops.findIndex(UINT64_MAX);
        for (; pic < ops.size();)
        {
            if (stop)
                break;
            uint64_t new_pic = runNextOp(ops[pic]);
            if (set_pic)
            {
                pic = new_pic;
                set_pic = false;
            }
            else
                pic++;
            if (new_pic == UINT64_MAX)
                break;
        }
    }
};
} // namespace LunaScript::lsvm