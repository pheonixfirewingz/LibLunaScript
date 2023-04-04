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
#include <filesystem>
#include <liblunascript/Compiler.h>
#include <libos/Defines.h>
#include <libos/FileIO.h>

const inline std::string createP(const std::string extend, const char *file_name,
                                 const char *file_ext = ".lls") noexcept
{
    std::string ret("$[asset_path]/");
    ret += extend;
    ret += file_name;
    ret += file_ext;
    return ret;
}

inline losResult fileRead(const std::string path, wchar_t **buf, data_size_t *buf_size) noexcept
{
    losFileHandle handle;
    losFileOpenInfo file;
    file.fileBits = LOS_FILE_BIT_READ | LOS_FILE_BIT_UNICODE;
    file.path = path.c_str();
    file.path_size = path.size();
    losResult res;
    if ((res = losOpenFile(&handle, file)) != LOS_SUCCESS)
        return res;
    if ((res = losReadFile(handle, (void **)buf, buf_size)) != LOS_SUCCESS)
        return res;
    if ((res = losCloseFile(handle)) != LOS_SUCCESS)
        return res;
    return LOS_SUCCESS;
}

int main(int, char **)
{
    libOSInit();
    losSetAssetPath(PROJECT_SOURCE_DIR);
    losResult res;
    //TODO: add unicode support to libos
    wchar_t *src;
    data_size_t src_size = 0;
    if ((res = fileRead(createP("", "test", ".lls"), &src, &src_size)) != LOS_SUCCESS)
        return res;
    testFeature(src, src_size);
    libOSCleanUp();
#    if ON_WINDOWS
    system("pause");
#    endif
    return 0;
}

/* int main(int, char **)
{
    libOSInit();
    losSetAssetPath(PROJECT_SOURCE_DIR);
    losResult res;
    char *src;
    data_size_t src_size = 0;
    if ((res = fileRead(createP("", "test", ".lls"), &src, &src_size)) != LOS_SUCCESS)
        return res;
    puts(std::string("\x1B[32mCompiler Output:\x1B[33m").c_str());
    LunaScriptCompiler compile(std::string(src, 0, src_size), true);
    delete src;
    if (compile.didScriptCompile() != LOS_SUCCESS)
    {
#    if ON_WINDOWS
        system("pause");
#    endif
        return compile.didScriptCompile();
    }
    puts(std::string("\x1B[32mAst:\x1B[33m\n").c_str());
    const std::string ast = compile.getJsonAST();
    puts(ast.c_str());
    libOSCleanUp();
#    if ON_WINDOWS
    system("pause");
#    endif
    return 0;
}*/
#endif