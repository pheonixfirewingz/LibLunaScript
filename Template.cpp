#include <filesystem>
#include <liblunascript/Compiler.h>
#include <libos/Defines.h>
#include <libos/FileIO.h>
#include <string>

#define GEN_TESTS

inline losResult fileRead(const std::string path, char **buf, data_size_t *buf_size) noexcept
{
    losFileHandle handle;
    losFileOpenInfo file;
    file.fileBits = LOS_FILE_BIT_READ;
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

template<typename T>
inline losResult fileWrite(const std::string path, const T *buf, const data_size_t buf_size) noexcept
{
    if (losDoseFileExist(path.c_str()) != LOS_SUCCESS)
    {
        try
        {
            std::filesystem::path directory_path = std::filesystem::path(path).parent_path();
            if (!std::filesystem::is_directory(directory_path))
                if (!std::filesystem::create_directories(directory_path))
                    return LOS_ERROR_MALFORMED_DATA;
        }
        catch (std::exception const &e)
        {
            puts(e.what());
            return LOS_ERROR_MALFORMED_DATA;
        }
    }
    losFileHandle handle;
    losFileOpenInfo file;
    if (losDoseFileExist(path.c_str()) != LOS_SUCCESS)
        file.fileBits = LOS_FILE_BIT_WRITE | LOS_FILE_BIT_CREATE;
    else
        file.fileBits = LOS_FILE_BIT_WRITE;
    file.path = path.c_str();
    file.path_size = path.size();
    losResult res;
    if ((res = losOpenFile(&handle, file)) != LOS_SUCCESS)
        return res;
    if ((res = losWriteFile(handle, buf, buf_size)) != LOS_SUCCESS)
        return res;
    if ((res = losCloseFile(handle)) != LOS_SUCCESS)
        return res;
    return LOS_SUCCESS;
}

inline void fileDelete(const char *path, const int path_size)
{
    losFileHandle handle;
    losFileOpenInfo file;
    file.fileBits = LOS_FILE_BIT_DELETE_AFTER_CLOSE;
    file.path = path;
    file.path_size = path_size;
    losResult res;
    if ((res = losOpenFile(&handle, file)) != LOS_SUCCESS)
        return;
    if ((res = losCloseFile(handle)) != LOS_SUCCESS)
        return;
}

const inline std::string createP(const std::string extend, const char *file_name,
                                 const char *file_ext = ".lls") noexcept
{
    std::string ret("$[asset_base]/");
    ret += extend;
    ret += "/";
    ret += file_name;
    ret += file_ext;
    return ret;
}

#include <gtest/gtest.h>
#ifdef GEN_TESTS
#    define AST_COMPILER_TEST(extend, test_name)                                                                   \
        TEST(AST, test_name)                                                                                       \
        {                                                                                                          \
            libOSInit();                                                                                           \
            losSetAssetPath(PROJECT_SOURCE_DIR);                                                                   \
            char *src = nullptr;                                                                                   \
            data_size_t src_size = 0;                                                                              \
            EXPECT_TRUE(fileRead(createP(std::string("tests/src") + extend, #test_name), &src, &src_size) ==       \
                        LOS_SUCCESS);                                                                              \
            LunaScriptCompiler compiler(std::string(src, 0, src_size), true);                                      \
            std::string ast = compiler.getJsonAST();                                                               \
            EXPECT_FALSE(compiler.didScriptCompile() != LOS_SUCCESS);                                              \
            EXPECT_TRUE(fileWrite(createP(std::string("tests/ast") + extend, #test_name, ".lls.ast"), ast.c_str(), \
                                  ast.size()) == LOS_SUCCESS);                                                     \
            libOSCleanUp();                                                                                        \
        }
#else
#    define AST_COMPILER_TEST(extend, test_name)                                                             \
        TEST(AST, test_name)                                                                                 \
        {                                                                                                    \
            libOSInit();                                                                                     \
            char *src = nullptr;                                                                             \
            data_size_t src_size = 0;                                                                        \
            EXPECT_TRUE(fileRead(createP(std::string("tests/src") + extend, #test_name), &src, &src_size) == \
                        LOS_SUCCESS);                                                                        \
            LunaScriptCompiler compiler(std::string(src, 0, src_size), true);                                \
            std::string err = compiler.getJsonAST();                                                         \
            EXPECT_FALSE(compiler.didScriptCompile() != LOS_SUCCESS);                                        \
            char *ast;                                                                                       \
            data_size_t ast_size = 0;                                                                        \
            EXPECT_TRUE(fileRead(createP(std::string("tests/ast") + extend, #test_name, ".lls.ast"), &ast,   \
                                 &ast_size) == LOS_SUCCESS);                                                 \
            EXPECT_STREQ(compiler.getJsonAST().c_str(), std::string(ast, 0, ast_size).c_str());              \
            libOSCleanUp();                                                                                  \
        }
#endif

#define AST_BAD_COMPILER_TEST(extend, test_name)                                                    \
    TEST(AST, test_name)                                                                            \
    {                                                                                               \
        libOSInit();                                                                                \
        losSetAssetPath(PROJECT_SOURCE_DIR);                                                        \
        [[maybe_unused]] Compiler compiler = nullptr;                                               \
        char *read_str_src = nullptr;                                                               \
        data_size_t read_str_src_size = 0;                                                          \
        EXPECT_TRUE(fileRead(createP(std::string("tests/src") + extend, #test_name), &read_str_src, \
                             &read_str_src_size) == LOS_SUCCESS);                                   \
        EXPECT_TRUE(compile(&compiler, read_str_src, read_str_src_size, false) != LOS_SUCCESS);     \
        freeCompiler(compiler);                                                                     \
        libOSCleanUp();                                                                             \
    }