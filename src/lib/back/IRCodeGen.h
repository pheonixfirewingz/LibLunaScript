#include "../AST.h"
#include <extend_std/Vector.h>
#include <map>
#include <memory>
#include <string>

namespace llvm
{
class Type;
class Value;
class Function;
class Module;
class LLVMContext;
class ConstantFolder;
class IRBuilderDefaultInserter;
template<typename FolderTy, typename InserterTy> class IRBuilder;
} // namespace llvm

namespace CodeGen
{
using namespace llvm;
class LLVMCodeGen
{
    std::shared_ptr<LLVMContext> context;
    std::shared_ptr<Module> _module_; // name is like this due to c++ 20(+) keyword module
    std::shared_ptr<IRBuilder<ConstantFolder, IRBuilderDefaultInserter>> builder;

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
        ContextAwareBlock(std::map<std::string, Value *> &named_vars, const IRType &type,
                          const ASTBlock *&block) noexcept
            : args({})
            , named_vars(named_vars)
            , return_type(type)
            , parent(nullptr)
            , block(block)
        {
        }
        ContextAwareBlock(const Function *func, const std::ReadOnlyVector<IRType> args, const IRType &type,
                          const ASTBlock *const &block) noexcept;
        ContextAwareBlock(const ContextAwareBlock *parent) noexcept
            : args(parent->args)
            , named_vars(std::map<std::string, Value *>())
            , return_type(parent->return_type)
            , parent(parent)
            , block(parent->block)
        {
        }
        static ContextAwareBlock *make(const Function *func, const std::ReadOnlyVector<IRType> args, const IRType &type,
                                       const ASTBlock *const &block) noexcept
        {
            return new (std::nothrow) ContextAwareBlock(func, args, type, block);
        }

        static ContextAwareBlock *make(const ContextAwareBlock *parent) noexcept
        {
            return new (std::nothrow) ContextAwareBlock(parent);
        }
    };

    inline bool isIntSigned(const ASTDataType type) const noexcept
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

    IRType getIRType(std::string name, ASTDataType type) const noexcept;
    std::vector<IRType> genArgs(const ASTExpression *args) noexcept;
    Value *genLiteral(const ASTLiteral *lit, const ContextAwareBlock *block, bool used_return_type = false) noexcept;
    Value *genBinary(const ASTBinaryExpression *op, const ContextAwareBlock *block) noexcept;
    bool genBlock(const ContextAwareBlock *block) noexcept;
    void genFunc(const ASTFuncDef *func) noexcept;
    void genCode(const ASTRoot *root, bool optimize) noexcept;

  public:
    LLVMCodeGen(const ASTRoot *root, bool optimize = false);
    ~LLVMCodeGen() = default;

    std::string getIR() noexcept;
};
} // namespace CodeGen