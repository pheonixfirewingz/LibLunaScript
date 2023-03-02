#include "VirtualMachine.h"
#include <liblunascript/Lsvm.h>

void run(VMData *vm, const std::ReadOnlyVector<uint64_t> ops,uint8_t debug_mode)
{
    LunaScript::lsvm::LunaScriptVirtualMachine runner(vm,ops,debug_mode);
}