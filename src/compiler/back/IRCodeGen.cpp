#include "IRCodeGen.h"
#include "InternalIRCodeGen.h"

IRType getIRType(std::string name, ASTDataType type)
{
    switch (type)
    {
    case INT8_TYPE:
        return (IRType){name, INT8_TYPE, Type::getInt8Ty(*context), true};
        break;
    case INT16_TYPE:
        return (IRType){name, INT16_TYPE, Type::getInt16Ty(*context), true};
        break;
    case INT32_TYPE:
        return (IRType){name, INT32_TYPE, Type::getInt32Ty(*context), true};
        break;
    case INT64_TYPE:
        return (IRType){name, INT64_TYPE, Type::getInt64Ty(*context), true};
        break;
    case UINT8_TYPE:
        return (IRType){name, UINT8_TYPE, Type::getInt8Ty(*context), false};
        break;
    case UINT16_TYPE:
        return (IRType){name, UINT16_TYPE, Type::getInt16Ty(*context), false};
        break;
    case UINT32_TYPE:
        return (IRType){name, UINT32_TYPE, Type::getInt32Ty(*context), false};
        break;
    case UINT64_TYPE:
        return (IRType){name, UINT64_TYPE, Type::getInt64Ty(*context), false};
        break;
    case FLOAT32_TYPE:
        return (IRType){name, FLOAT32_TYPE, Type::getFloatTy(*context), false};
        break;
    case FLOAT64_TYPE:
        return (IRType){name, FLOAT32_TYPE, Type::getDoubleTy(*context), false};
        break;
    default:
        return (IRType){{}, NOT_DETERMINED_DATA_TYPE, NULL, false};
    }
}

std::vector<IRType> genArgs(const ASTExpression *args)
{
    std::vector<IRType> ir_args;
    for (auto item : args->list)
    {
        auto arg = (const ASTLiteral *)item;
        ir_args.push_back(getIRType(arg->value, arg->data_type));
    }
    return ir_args;
}

Value *genLiteral(std::shared_ptr<IRBuilder<>>, const ASTLiteral *lit, const ContextAwareBlock *block,
                  bool used_return_type = false)
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

Value *genBinary(std::shared_ptr<IRBuilder<>> builder, const ASTBinaryExpression *op, const ContextAwareBlock *block)
{
    Value *L = genLiteral(builder, (const ASTLiteral *)op->left, block);
    Value *R = nullptr;
    if (op->right->getTypeID() == AST_BINARY)
        R = genBinary(builder, (const ASTBinaryExpression *)op->right, block);
    else
        R = genLiteral(builder, (const ASTLiteral *)op->right, block);
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

bool genBlock(std::shared_ptr<IRBuilder<>> builder, const ContextAwareBlock *block)
{
    for (auto &action : block->block->list)
    {
        switch (action->type)
        {
        case AST_EXPR_RETURN: {
            Value *ret;
            if (action->list[0]->getTypeID() == AST_BINARY)
                ret = genBinary(builder, (const ASTBinaryExpression *)action->list[0], block);
            else
                ret = genLiteral(builder, (const ASTLiteral *)action->list[0], block, true);
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

void genFunc(std::shared_ptr<IRBuilder<>> builder, Module **module, const ASTFuncDef *func)
{
    Function *ir_func = (*module)->getFunction(func->name);
    std::vector<IRType> args = genArgs(func->args);
    if (!ir_func)
    {
        std::vector<Type *> ir_args;
        for (auto arg : args)
            ir_args.push_back(arg.type);
        FunctionType *func_type = FunctionType::get(getIRType({}, func->return_type).type, ir_args, false);

        ir_func = Function::Create(func_type, Function::InternalLinkage, func->name, *module);
        unsigned Idx = 0;
        for (auto &Arg : ir_func->args())
            Arg.setName(((const ASTLiteral *)func->args->list[Idx++])->value);
    }

    if (!ir_func)
        return;

    BasicBlock *BB = BasicBlock::Create(*context, "entry", ir_func);
    builder->SetInsertPoint(BB);
    if (genBlock(builder, ContextAwareBlock::make(ir_func, std::ReadOnlyVector<IRType>::lock(args),
                                                  getIRType({}, func->return_type), func->body)))
        verifyFunction(*ir_func);
    else
        ir_func->eraseFromParent();
}

Module *generateIRCode(const ASTRoot *root) noexcept
{
    if (!context)
        context = std::make_shared<LLVMContext>();
    std::shared_ptr<IRBuilder<>> builder = std::make_shared<IRBuilder<>>(*context);
    Module *module = new (std::nothrow) Module(root->name, *context);

    for (auto child : root->children)
    {
        if (child->getTypeID() == AST_FUNC_DEF)
            genFunc(builder, &module, (const ASTFuncDef *)child);
    }

    return module;
}