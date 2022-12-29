#pragma once
#include <algorithm>
#include <extend_std/Vector.h>
#include <llvm-15/llvm/IR/IRBuilder.h>
#include <llvm-15/llvm/IR/Verifier.h>
#include <map>
#include <memory>

using namespace llvm;

// this is a bad person move holding in global state
static std::shared_ptr<LLVMContext> context;

struct IRType
{
    std::string name;
    ASTDataType data_type;
    Type *type;
    bool isSigned;

    constexpr IRType() noexcept
        : name("")
        , data_type(NOT_DETERMINED_DATA_TYPE)
        , type(nullptr)
        , isSigned(false)
    {
    }

    explicit constexpr IRType(std::string name, ASTDataType data_type, Type *type, bool isSigned) noexcept
        : name(name)
        , data_type(data_type)
        , type(type)
        , isSigned(isSigned)
    {
    }
};

struct ContextAwareBlock
{
    const std::ReadOnlyVector<IRType> args;
    std::map<std::string, Value *> named_vars;
    const IRType return_type;
    const ContextAwareBlock *parent;
    const ASTBlock *block;
    ContextAwareBlock(std::map<std::string, Value *> &named_vars, const IRType &type, const ASTBlock *&block) noexcept
        : args({})
        , named_vars(named_vars)
        , return_type(type)
        , parent(nullptr)
        , block(block)
    {
    }
    ContextAwareBlock(const Function *func, const std::ReadOnlyVector<IRType> args, const IRType &type,
                      const ASTBlock *const &block) noexcept
        : args(args)
        , named_vars(std::map<std::string, Value *>())
        , return_type(type)
        , parent(nullptr)
        , block(block)
    {
        for (auto &Arg : func->args())
            named_vars[std::string(Arg.getName())] = (Value *)&Arg;
    }
    ContextAwareBlock(const ContextAwareBlock *parent) noexcept
        : args(parent->args)
        , named_vars(std::map<std::string, Value *>())
        , return_type(parent->return_type)
        , parent(parent)
        , block(parent->block)
    {
    }
    static ContextAwareBlock *make(const Function *func, const std::ReadOnlyVector<IRType> args, const IRType &type,
                                   const ASTBlock *const &block)
    {
        return new (std::nothrow) ContextAwareBlock(func, args, type, block);
    }

    static ContextAwareBlock *make(const ContextAwareBlock *parent)
    {
        return new (std::nothrow) ContextAwareBlock(parent);
    }
};

inline bool isIntSigned(const ASTDataType type) noexcept
{
    switch (type)
    {
    case NOT_DETERMINED_DATA_TYPE:
    case VOID_TYPE:
    case FLOAT32_TYPE:
    case FLOAT64_TYPE:
    case FLOAT_ANY_TYPE:
    case UINT8_TYPE:
    case UINT16_TYPE:
    case UINT32_TYPE:
    case UINT64_TYPE:
    case UINT_ANY_TYPE:
        return false;
    case INT8_TYPE:
    case INT16_TYPE:
    case INT32_TYPE:
    case INT64_TYPE:
    case INT_ANY_TYPE:
        return true;
    }
}