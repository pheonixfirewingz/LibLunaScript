#include "IRCodeGen.h"
#include <llvm-15/llvm/ADT/Optional.h>
#include <llvm-15/llvm/IR/IRBuilder.h>
#include <llvm-15/llvm/IR/LegacyPassManager.h>
#include <llvm-15/llvm/IR/Verifier.h>
#include <llvm-15/llvm/MC/TargetRegistry.h>
#include <llvm-15/llvm/Passes/PassBuilder.h>
#include <llvm-15/llvm/Support/Host.h>
#include <llvm-15/llvm/Support/TargetSelect.h>
#include <llvm-15/llvm/Target/TargetMachine.h>

namespace CodeGen
{

LLVMCodeGen::ContextAwareBlock::ContextAwareBlock(const Function *func,
                                                  const std::ReadOnlyVector<LLVMCodeGen::IRType> args,
                                                  const LLVMCodeGen::IRType &type,
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

LLVMCodeGen::IRType LLVMCodeGen::getIRType(std::string name, ASTDataType type) const noexcept
{
    switch (type)
    {
    case INT8_TYPE:
        return (IRType){name, INT8_TYPE, Type::getInt8Ty(*context), true};
    case INT16_TYPE:
        return (IRType){name, INT16_TYPE, Type::getInt16Ty(*context), true};
    case INT32_TYPE:
        return (IRType){name, INT32_TYPE, Type::getInt32Ty(*context), true};
    case INT64_TYPE:
        return (IRType){name, INT64_TYPE, Type::getInt64Ty(*context), true};
    case UINT8_TYPE:
        return (IRType){name, UINT8_TYPE, Type::getInt8Ty(*context), false};
    case UINT16_TYPE:
        return (IRType){name, UINT16_TYPE, Type::getInt16Ty(*context), false};
    case UINT32_TYPE:
        return (IRType){name, UINT32_TYPE, Type::getInt32Ty(*context), false};
    case UINT64_TYPE:
        return (IRType){name, UINT64_TYPE, Type::getInt64Ty(*context), false};
    case FLOAT32_TYPE:
        return (IRType){name, FLOAT32_TYPE, Type::getFloatTy(*context), false};
    case FLOAT64_TYPE:
        return (IRType){name, FLOAT32_TYPE, Type::getDoubleTy(*context), false};
    case VOID_TYPE:
        return (IRType){name, VOID_TYPE, Type::getVoidTy(*context), false};
    default:
        return (IRType){{}, NOT_DETERMINED_DATA_TYPE, NULL, false};
    }
}

std::vector<LLVMCodeGen::IRType> LLVMCodeGen::genArgs(const ASTExpression *args) noexcept
{
    std::vector<IRType> ir_args;
    for (auto item : args->list)
    {
        auto arg = (const ASTLiteral *)item;
        ir_args.push_back(getIRType(arg->value, arg->data_type));
    }
    return ir_args;
}

Value *LLVMCodeGen::genLiteral(const ASTLiteral *lit, const ContextAwareBlock *block, bool used_return_type) noexcept
{
    Value *ret;
    if (lit->data_type == NOT_DETERMINED_DATA_TYPE)
        ret = block->named_vars.find(lit->value)->second;
    else
    {
        ASTDataType data_type;
        if (used_return_type)
            data_type = block->return_type.data_type;
        else
            data_type = lit->data_type;

        switch (data_type)
        {
        case FLOAT32_TYPE:
            ret = ConstantFP::get(*context, APFloat(stof(lit->value)));
            break;
        case FLOAT64_TYPE:
            ret = ConstantFP::get(*context, APFloat(stod(lit->value)));
            break;
        case FLOAT_ANY_TYPE:
            ret = ConstantFP::get(*context, APFloat(stod(lit->value)));
            break;
        case INT8_TYPE:
            ret = ConstantInt::get(*context, APInt(8, stoi(lit->value), true));
            break;
        case INT16_TYPE:
            ret = ConstantInt::get(*context, APInt(16, stoi(lit->value), true));
            break;
        case INT32_TYPE:
            ret = ConstantInt::get(*context, APInt(32, stoi(lit->value), true));
            break;
        case INT64_TYPE:
            ret = ConstantInt::get(*context, APInt(64, stoi(lit->value), true));
            break;
        case UINT8_TYPE:
            ret = ConstantInt::get(*context, APInt(8, stoi(lit->value)));
            break;
        case UINT16_TYPE:
            ret = ConstantInt::get(*context, APInt(16, stoi(lit->value)));
            break;
        case UINT32_TYPE:
            ret = ConstantInt::get(*context, APInt(32, stoi(lit->value)));
            break;
        case UINT64_TYPE:
            ret = ConstantInt::get(*context, APInt(64, stoi(lit->value)));
            break;
        case UINT_ANY_TYPE:
            ret = ConstantInt::get(*context, APInt(64, stoi(lit->value)));
            break;
        case INT_ANY_TYPE:
            ret = ConstantInt::get(*context, APInt(64, stoi(lit->value), true));
            break;
        case NOT_DETERMINED_DATA_TYPE:
        case VOID_TYPE:
            ret = nullptr;
            break;
        }
    }
    return ret;
}

Value *LLVMCodeGen::genBinary(const ASTBinaryExpression *op, const ContextAwareBlock *block) noexcept
{
    Value *L = genLiteral((const ASTLiteral *)op->left, block);
    Value *R = nullptr;
    if (op->right->getTypeID() == AST_BINARY)
        R = genBinary( (const ASTBinaryExpression *)op->right, block);
    else
        R = genLiteral( (const ASTLiteral *)op->right, block);
    if (!L || !R)
        return nullptr;

    switch (op->op)
    {
    case ADD_TYPE:
        return builder->CreateAdd(L, R);
    case SUB_TYPE:
        return builder->CreateSub(L, R);
    case DIV_TYPE: {
        if (((const ASTLiteral *)op->left)->data_type == NOT_DETERMINED_DATA_TYPE)
        {
            for (auto &item : block->args)
            {
                if (item.name == ((const ASTLiteral *)op->left)->value)
                {
                    if (item.isSigned)
                        return builder->CreateSDiv(L, R);
                    else
                        return builder->CreateUDiv(L, R);
                }
            }
        }
        else
        {
            if (isIntSigned(((const ASTLiteral *)op->left)->data_type))
                return builder->CreateSDiv(L, R);
            else
                return builder->CreateUDiv(L, R);
        }
    }
    case MUL_TYPE:
        return builder->CreateMul(L, R);
    case AND_TYPE:
        return builder->CreateAnd(L, R);
    case OR_TYPE:
        return builder->CreateOr(L, R);
    case XOR_TYPE:
        return builder->CreateXor(L, R);
    case MOD_TYPE:
        puts("module not implemented on ir gen");
    case NOT_DETERMINED_OP_TYPE:
        break;
    }
    return NULL;
}

bool LLVMCodeGen::genBlock(const ContextAwareBlock*block) noexcept
{
    for (auto &action : block->block->list)
    {
        switch (action->type)
        {
        case AST_EXPR_RETURN: {
            Value *ret;
            if (action->list[0]->getTypeID() == AST_BINARY)
                ret = genBinary((const ASTBinaryExpression *)action->list[0], block);
            else
                ret = genLiteral((const ASTLiteral *)action->list[0], block, true);
            builder->CreateRet(ret);
        }
        case AST_EXPR_VAR_DEFINED: {
        }
        default:
            return true;
        }
    }
    return false;
}

void LLVMCodeGen::genFunc(const ASTFuncDef *func) noexcept
{
    Function *ir_func = _module_->getFunction(func->name);
    std::vector<IRType> args = genArgs(func->args);
    if (!ir_func)
    {
        std::vector<Type *> ir_args;
        for (auto arg : args)
            ir_args.push_back(arg.type);
        FunctionType *func_type = FunctionType::get(getIRType({}, func->return_type).type, ir_args, false);

        GlobalValue::LinkageTypes link_type =
            func->is_public ? Function::AvailableExternallyLinkage : Function::InternalLinkage;
        ir_func = Function::Create(func_type, link_type, func->name, *_module_);
        ir_func->setCallingConv(CallingConv::C);
        AttributeList list;

        ir_func->setAttributes(list);
        unsigned Idx = 0;
        for (auto &Arg : ir_func->args())
            Arg.setName(((const ASTLiteral *)func->args->list[Idx++])->value);
    }

    if (!ir_func)
        return;

    BasicBlock *BB = BasicBlock::Create(*context, "", ir_func);
    builder->SetInsertPoint(BB);
    if (genBlock(LLVMCodeGen::ContextAwareBlock::make(ir_func, std::ReadOnlyVector<IRType>::lock(args),
                                                               getIRType({}, func->return_type), func->body)))
        verifyFunction(*ir_func);
    else
        ir_func->eraseFromParent();
}

void LLVMCodeGen::genCode(const ASTRoot *root, bool optimize) noexcept
{
    for (auto child : root->children)
    {
        if (child->getTypeID() == AST_FUNC_DEF)
            genFunc((const ASTFuncDef *)child);
    }

    if (optimize)
    {
        LoopAnalysisManager loop_analysis;
        CGSCCAnalysisManager global_analysis;
        FunctionAnalysisManager func_analysis;
        ModuleAnalysisManager module_analysis;
        PassBuilder pass_builder;

        pass_builder.registerModuleAnalyses(module_analysis);
        pass_builder.registerFunctionAnalyses(func_analysis);
        pass_builder.registerLoopAnalyses(loop_analysis);
        pass_builder.crossRegisterProxies(loop_analysis, func_analysis, global_analysis, module_analysis);

        ModulePassManager module_pass_manager = pass_builder.buildPerModuleDefaultPipeline(OptimizationLevel::O1);
        module_pass_manager.run(*_module_, module_analysis);
    }
}

LLVMCodeGen::LLVMCodeGen(const ASTRoot *root, bool optimize)
{
    context = std::make_shared<LLVMContext>();
    builder = std::make_shared<IRBuilder<>>(*context);
    _module_ = std::make_shared<Module>(root->name, *context);
    InitializeAllTargetInfos();
    InitializeAllTargets();
    InitializeAllTargetMCs();
    InitializeAllAsmParsers();
    InitializeAllAsmPrinters();
    genCode(root, optimize);
}

std::string LLVMCodeGen::getIR() noexcept
{
    std::string ret;
    raw_string_ostream buffer(ret);
    _module_->print(buffer, nullptr);
    buffer.flush();
    return ret;
}

} // namespace CodeGen