#include "VirtualMachine.h"
#include <liblunascript/Lsvm.h>

void run(VMData *vm, const std::ReadOnlyVector<uint64_t> ops)
{
    LunaScript::lsvm::LunaScriptVirtualMachine runner(vm,ops);
}