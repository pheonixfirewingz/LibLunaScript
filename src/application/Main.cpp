#include "Cmake.h"
#include "IO.hpp"
#include <algorithm>
#include <argparse/argparse.hpp>
#include <cstdio>
#include <liblunascript/Compiler.h>
#include <string>

#if FUZZING == 1
extern "C++" int LLVMFuzzerTestOneInputI(const uint8_t *Data, size_t Size)
{
    LunaScriptCompiler compile(std::string((const char *)Data, 0, Size), "test");
    if (compile.didScriptCompile() != LOS_SUCCESS)
        return -1;
    return 0; // Values other than 0 and -1 are reserved for future use.
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
    return LLVMFuzzerTestOneInputI(Data, Size);
}
#else

int main(int, char **)
{
    libOSInit();
    losResult res;
    setRoot(PROJECT_SOURCE_DIR);
    char *src;
    data_size_t src_size = 0;
    if ((res = fileRead(createP("", "test", ".lls"), &src, &src_size)) != LOS_SUCCESS)
        return res;
    puts(std::string("\x1B[32mCompiler Output:\x1B[33m").c_str());
    LunaScriptCompiler compile(std::string(src, 0, src_size));
    delete src;
    if (compile.didScriptCompile() != LOS_SUCCESS)
    {
        puts(compile.getErrors().c_str());
#    if WIN32
        system("pause");
#    endif
        return compile.didScriptCompile();
    }
    puts(std::string("\x1B[32mAst:\x1B[33m\n").c_str());
    const char* ast = compile.getJsonAST().c_str();
    puts(ast);
    libOSCleanUp();
    #if WIN32
    system("pause");
    #endif
    return 0;
}
#endif