#include "Cmake.h"
#include "IO.hpp"
#include <argparse/argparse.hpp>
#include <cstdio>
#include <liblunascript/Assembler.h>
#include <liblunascript/Compiler.h>
#include <liblunascript/Lsvm.h>
#include <string>

struct ByteCode
{
    uint64_t op : 5;
    uint64_t reg : 3;
    uint64_t is_reg : 1;
    uint64_t is_constant : 1;
    uint64_t reg_or_memory_dest : 53;
    uint64_t reserved_0 : 1;
};

union Data {
    uint64_t value;
    ByteCode op;
};

losResult compile(const std::string &filename, const std::string &src, char **output, data_size_t *output_size) noexcept
{
    losResult res;
    Compiler compiler;
    if ((res = compile(&compiler, src.c_str(), src.size(), filename.c_str(), filename.size())) != LOS_SUCCESS)
    {
        while (hasErrorOnStack(compiler))
        {
            char *read_str;
            data_size_t read_str_size = 0;
            getErrorOffStack(compiler, &read_str, &read_str_size);
            puts(std::string(read_str, 0, read_str_size).c_str());
            free(read_str);
        }
        return res;
    }
    astToString(compiler, output, output_size);
    freeCompiler(compiler);
    return LOS_SUCCESS;
}

losResult assemble(const std::string &src, std::ReadOnlyVector<uint64_t> *ops) noexcept
{
    ops->copy(assemble(src.c_str(), src.size()));
    if (ops->empty())
        return LOS_ERROR_MALFORMED_DATA;
    return LOS_SUCCESS;
}

// int main(int argc, char **argv)
int main(int, char **)
{
    libOSInit();
    losResult res;
    setRoot(PROJECT_SOURCE_DIR);
    char *read_str;
    data_size_t read_str_size = 0;
    if ((res = fileRead(createP("", "test", ".llsb"), &read_str, &read_str_size)) != LOS_SUCCESS)
        return res;
    std::ReadOnlyVector<uint64_t> ops = {};
    if ((res = assemble(std::string(read_str, 0, read_str_size), &ops)) != LOS_SUCCESS)
        return res;
    if ((res = fileWrite<uint64_t>(createP("", "test", ".lsobj"), ops.data(), ops.size())) != LOS_SUCCESS)
        return res;
    VMData data{.vmCallbacks = {{VMFunctionName("vmExit").hash,
                                 [](std::stack<vm_data_t> *stack) {
                                     uint64_t typed_reg = std::get<uint64_t>(stack->top());
                                     stack->pop();
                                     printf("LunaScript - EndCode: %lu\n", typed_reg);
                                     return nullptr;
                                 }}},
                .vmErrorCallback = [](const char *msg) {
                    printf("LunaScript - ERROR: %s\n", msg);
                    std::exit(1);
                }};
    run(&data, ops, true);
    libOSCleanUp();
    return 0;
}
#if FUZZING == 1
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
    enableSoftErrors();
    std::string x((char *)Data, 0, Size);
    const ASTRoot *v = parse(std::move(x));
    delete v;
    int ret = hasErrors() ? 0 : -1;
    if (ret == -1)
        puts("did error correctly");
    disableSoftErrors();
    return ret; // Values other than 0 and -1 are reserved for future use.
}
#endif