#include "../Cmake.h"
#include "front/Parser.h"
#include <libos/FileIO.h>
#include <string>
#if COMPILER_TESTING == 0 && CMAKE_LIB_MODE == 0
#    include "back/IRCodeGen.h"
losResult fileRead(const char *path, const int path_size, char **buf, data_size_t *buf_size)
{
    losFileHandle handle;
    losFileOpenInfo file;
    file.fileBits = LOS_FILE_BIT_READ | LOS_FILE_BIT_CREATE;
    file.path = path;
    file.path_size = path_size;
    losResult res;
    if ((res = losOpenFile(&handle, file)) != LOS_SUCCESS)
        return res;
    if ((res = losReadFile(handle, (void **)buf, buf_size)) != LOS_SUCCESS)
        return res;
    if ((res = losCloseFile(handle)) != LOS_SUCCESS)
        return res;
    return LOS_SUCCESS;
}

losResult fileWrite(const char *path, const int path_size, const char *buf, data_size_t buf_size)
{
    losFileHandle handle;
    losFileOpenInfo file;
    file.fileBits = LOS_FILE_BIT_WRITE | LOS_FILE_BIT_CREATE;
    file.path = path;
    file.path_size = path_size;
    losResult res;
    if ((res = losOpenFile(&handle, file)) != LOS_SUCCESS)
        return res;
    if ((res = losWriteFile(handle, buf, buf_size)) != LOS_SUCCESS)
        return res;
    if ((res = losCloseFile(handle)) != LOS_SUCCESS)
        return res;
    return LOS_SUCCESS;
}

int main(int, char **)
{
    bool stop = false;
    losSetAssetPath("home/digitech/Desktop/LibLunaScript");
    losResult res;
    char *read_str;
    data_size_t read_str_size = 0;
    if ((res = fileRead("$[asset_base]/test.lls", 23, &read_str, &read_str_size)) != LOS_SUCCESS)
        return res;
    enableSoftErrors();
    const ASTRoot *root_ast = parse(std::move(std::string((char *)read_str, 0, read_str_size)), "test");
    if (hasErrors())
    {
        stop = true;
        while (hasErrors())
            printf("COMPILER ERROR: %s\n", popSoftErrorOffStack());
    }
    disableSoftErrors();
    if (stop)
        return 0;
    auto ast = toJson(root_ast, true);
    if ((res = fileWrite("$[asset_base]/test.lls.ast", 27, ast.c_str(), ast.size())) != LOS_SUCCESS)
        return res;
    auto ir = generateIRCode(root_ast);
    auto human_readable = makeHumanReadable(ir);
    if ((res = fileWrite("$[asset_base]/test.ll", 23, human_readable.c_str(), human_readable.size())) != LOS_SUCCESS)
        return res;
    return 0;
}
#elif FUZZING == 1
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
#elif CMAKE_LIB_MODE == 1
#endif