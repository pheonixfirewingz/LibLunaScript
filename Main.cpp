#include <liblunascript/Compiler.h>
#include <libos/Defines.h>
#include <libos/FileIO.h>
#include <string>

inline void setRoot(const char *root) noexcept
{
    losSetAssetPath(root);
}

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
    ret += file_name;
    ret += file_ext;
    return ret;
}
#if ENABLE_TESTS
#include <gtest/gtest.h>
#    define COMPILER_TEST(group, extend, test_name)                                                           \
        TEST(group, test_name)                                                                                \
        {                                                                                                     \
            libOSInit();                                                                                      \
            setRoot(PROJECT_SOURCE_DIR);                                                                      \
            char *src = nullptr;                                                                                        \
            data_size_t src_size = 0;                                                                         \
            EXPECT_TRUE(fileRead(createP(std::string("tests/src/") + extend, #test_name), &src, &src_size) == \
                        LOS_SUCCESS);                                                                         \
            LunaScriptCompiler compiler(std::string(src, 0, src_size),true);                                       \
            std::string err = compiler.getJsonAST();                                                          \
            EXPECT_FALSE(compiler.didScriptCompile() != LOS_SUCCESS);                                         \
            fileWrite<char>(createP(std::string("tests/ast/") + extend, #test_name, ".lls.ast"), err.c_str(), \
                            err.size());                                                                      \
            libOSCleanUp();                                                                                   \
        }
//
/*char *ast;                                                                                                  \
data_size_t ast_size = 0;                                                                                     \
EXPECT_TRUE(fileRead(createP(std::string("tests/ast/") + extend, #test_name, ".lls.ast"), &ast, &ast_size) == \
            LOS_SUCCESS);                                                                                     \
EXPECT_STREQ(compiler.getJsonAST().c_str(), std::string(ast, 0, ast_size).c_str());                           \*/

#    define BAD_COMPILER_TEST(group, extend, test_name)                                                  \
        TEST(group, test_name)                                                                           \
        {                                                                                                \
            libOSInit();                                                                                 \
            Compiler [[maybe_unused]] compiler = nullptr;                                                \
            setRoot(PROJECT_SOURCE_DIR);                                                                 \
            char *read_str_src = nullptr;                                                                          \
            data_size_t read_str_src_size = 0;                                                           \
            EXPECT_TRUE(fileRead(createP(std::string("tests/src/") + extend, #test_name), &read_str_src, \
                                 &read_str_src_size) == LOS_SUCCESS);                                    \
            EXPECT_TRUE(compile(&compiler, read_str_src, read_str_src_size, false) != LOS_SUCCESS);      \
            freeCompiler(compiler);                                                                      \
            libOSCleanUp();                                                                              \
        }
// variables without set value
COMPILER_TEST(AST, "var/no_value/", parse_var_uint8_no_value)
COMPILER_TEST(AST, "var/no_value/", parse_var_uint16_no_value)
COMPILER_TEST(AST, "var/no_value/", parse_var_uint32_no_value)
COMPILER_TEST(AST, "var/no_value/", parse_var_uint64_no_value)
COMPILER_TEST(AST, "var/no_value/", parse_var_int8_no_value)
COMPILER_TEST(AST, "var/no_value/", parse_var_int16_no_value)
COMPILER_TEST(AST, "var/no_value/", parse_var_int32_no_value)
COMPILER_TEST(AST, "var/no_value/", parse_var_int64_no_value)
COMPILER_TEST(AST, "var/no_value/", parse_var_float32_no_value)
COMPILER_TEST(AST, "var/no_value/", parse_var_float64_no_value)
// variables with set value
COMPILER_TEST(AST, "var/value/", parse_var_uint8_with_value)
COMPILER_TEST(AST, "var/value/", parse_var_uint16_with_value)
COMPILER_TEST(AST, "var/value/", parse_var_uint32_with_value)
COMPILER_TEST(AST, "var/value/", parse_var_uint64_with_value)
COMPILER_TEST(AST, "var/value/", parse_var_int8_with_value)
COMPILER_TEST(AST, "var/value/", parse_var_int16_with_value)
COMPILER_TEST(AST, "var/value/", parse_var_int32_with_value)
COMPILER_TEST(AST, "var/value/", parse_var_int64_with_value)
COMPILER_TEST(AST, "var/value/", parse_var_float32_with_value)
COMPILER_TEST(AST, "var/value/", parse_var_float64_with_value)
// variables of a global nature
COMPILER_TEST(AST, "var/globals/no_value/", parse_global_var_uint8_no_value)
COMPILER_TEST(AST, "var/globals/no_value/", parse_global_var_uint16_no_value)
COMPILER_TEST(AST, "var/globals/no_value/", parse_global_var_uint32_no_value)
COMPILER_TEST(AST, "var/globals/no_value/", parse_global_var_uint64_no_value)
COMPILER_TEST(AST, "var/globals/no_value/", parse_global_var_int8_no_value)
COMPILER_TEST(AST, "var/globals/no_value/", parse_global_var_int16_no_value)
COMPILER_TEST(AST, "var/globals/no_value/", parse_global_var_int32_no_value)
COMPILER_TEST(AST, "var/globals/no_value/", parse_global_var_int64_no_value)
COMPILER_TEST(AST, "var/globals/no_value/", parse_global_var_float32_no_value)
COMPILER_TEST(AST, "var/globals/no_value/", parse_global_var_float64_no_value)
// variables with binary expression
COMPILER_TEST(AST, "var/binary/", parse_var_binary_expr_1)
COMPILER_TEST(AST, "var/binary/", parse_var_binary_expr_2)
COMPILER_TEST(AST, "var/binary/", parse_var_binary_expr_3)
COMPILER_TEST(AST, "var/binary/", parse_var_binary_expr_4)
COMPILER_TEST(AST, "var/binary/", parse_var_binary_expr_5)
COMPILER_TEST(AST, "var/binary/", parse_var_binary_expr_6)
COMPILER_TEST(AST, "var/binary/", parse_var_binary_expr_7)
COMPILER_TEST(AST, "var/binary/", parse_var_binary_expr_8)
COMPILER_TEST(AST, "var/binary/", parse_var_binary_expr_9)
COMPILER_TEST(AST, "var/binary/", parse_var_binary_expr_10)
COMPILER_TEST(AST, "var/binary/", parse_var_binary_expr_11)
COMPILER_TEST(AST, "var/binary/", parse_var_binary_expr_12)
COMPILER_TEST(AST, "var/binary/", parse_var_binary_expr_13)
COMPILER_TEST(AST, "var/binary/", parse_var_binary_expr_14)
COMPILER_TEST(AST, "var/binary/", parse_var_binary_expr_15)
COMPILER_TEST(AST, "var/binary/", parse_var_binary_expr_16)
COMPILER_TEST(AST, "var/binary/", parse_var_binary_expr_17)
COMPILER_TEST(AST, "var/binary/", parse_var_binary_expr_18)
// bad syntax
BAD_COMPILER_TEST(AST, "var/bad_syntax/", bad_syntax_1)
BAD_COMPILER_TEST(AST, "var/bad_syntax/", bad_syntax_2)
BAD_COMPILER_TEST(AST, "var/bad_syntax/", bad_syntax_3)
BAD_COMPILER_TEST(AST, "var/bad_syntax/", bad_syntax_4)
BAD_COMPILER_TEST(AST, "var/bad_syntax/", bad_syntax_5)
BAD_COMPILER_TEST(AST, "var/bad_syntax/", bad_syntax_6)
BAD_COMPILER_TEST(AST, "var/bad_syntax/", bad_syntax_7)
BAD_COMPILER_TEST(AST, "var/bad_syntax/", bad_syntax_8)
BAD_COMPILER_TEST(AST, "var/bad_syntax/", bad_syntax_9)
BAD_COMPILER_TEST(AST, "var/bad_syntax/", bad_syntax_10)
BAD_COMPILER_TEST(AST, "var/bad_syntax/", bad_syntax_11)
BAD_COMPILER_TEST(AST, "var/bad_syntax/", bad_syntax_12)
BAD_COMPILER_TEST(AST, "var/bad_syntax/", bad_syntax_13)
BAD_COMPILER_TEST(AST, "var/bad_syntax/", bad_syntax_14)
BAD_COMPILER_TEST(AST, "var/bad_syntax/", bad_syntax_15)
BAD_COMPILER_TEST(AST, "var/bad_syntax/", bad_syntax_16)
// function basics
COMPILER_TEST(AST, "func/common/", parse_main_func)
COMPILER_TEST(AST, "func/common/", parse_main_with_no_return)
COMPILER_TEST(AST, "func/common/", parse_main_with_no_return_used_return)
COMPILER_TEST(AST, "func/common/", parse_main_with_another_function_no_call)
COMPILER_TEST(AST, "func/common/", parse_main_with_another_function_call)
//  function basics - variables with set value
COMPILER_TEST(AST, "func/var/value/", parse_main_func_with_uint8_with_value)
COMPILER_TEST(AST, "func/var/value/", parse_main_func_with_uint16_with_value)
COMPILER_TEST(AST, "func/var/value/", parse_main_func_with_uint32_with_value)
COMPILER_TEST(AST, "func/var/value/", parse_main_func_with_uint64_with_value)
COMPILER_TEST(AST, "func/var/value/", parse_main_func_with_int8_with_value)
COMPILER_TEST(AST, "func/var/value/", parse_main_func_with_int16_with_value)
COMPILER_TEST(AST, "func/var/value/", parse_main_func_with_int32_with_value)
COMPILER_TEST(AST, "func/var/value/", parse_main_func_with_int64_with_value)
COMPILER_TEST(AST, "func/var/value/", parse_main_func_with_float32_with_value)
COMPILER_TEST(AST, "func/var/value/", parse_main_func_with_float64_with_value)
// // function basics - variables without set value
COMPILER_TEST(AST, "func/var/no_value/", parse_main_func_with_uint8_with_no_value)
COMPILER_TEST(AST, "func/var/no_value/", parse_main_func_with_uint16_with_no_value)
COMPILER_TEST(AST, "func/var/no_value/", parse_main_func_with_uint32_with_no_value)
COMPILER_TEST(AST, "func/var/no_value/", parse_main_func_with_uint64_with_no_value)
COMPILER_TEST(AST, "func/var/no_value/", parse_main_func_with_int8_with_no_value)
COMPILER_TEST(AST, "func/var/no_value/", parse_main_func_with_int16_with_no_value)
COMPILER_TEST(AST, "func/var/no_value/", parse_main_func_with_int32_with_no_value)
COMPILER_TEST(AST, "func/var/no_value/", parse_main_func_with_int64_with_no_value)
COMPILER_TEST(AST, "func/var/no_value/", parse_main_func_with_float32_with_no_value)
COMPILER_TEST(AST, "func/var/no_value/", parse_main_func_with_float64_with_no_value)
// function basics - variables with binary expression
COMPILER_TEST(AST, "func/var/binary/", parse_main_func_with_binary_expr_1)
COMPILER_TEST(AST, "func/var/binary/", parse_main_func_with_binary_expr_2)
COMPILER_TEST(AST, "func/var/binary/", parse_main_func_with_binary_expr_3)
COMPILER_TEST(AST, "func/var/binary/", parse_main_func_with_binary_expr_4)
COMPILER_TEST(AST, "func/var/binary/", parse_main_func_with_binary_expr_5)
COMPILER_TEST(AST, "func/var/binary/", parse_main_func_with_binary_expr_6)
COMPILER_TEST(AST, "func/var/binary/", parse_main_func_with_binary_expr_7)
COMPILER_TEST(AST, "func/var/binary/", parse_main_func_with_binary_expr_8)
// function basics - bad syntax
BAD_COMPILER_TEST(AST, "func/bad_syntax/", func_bad_syntax_1)
BAD_COMPILER_TEST(AST, "func/bad_syntax/", func_bad_syntax_2)
BAD_COMPILER_TEST(AST, "func/bad_syntax/", func_bad_syntax_3)
BAD_COMPILER_TEST(AST, "func/bad_syntax/", func_bad_syntax_4)
BAD_COMPILER_TEST(AST, "func/bad_syntax/", func_bad_syntax_5)
BAD_COMPILER_TEST(AST, "func/bad_syntax/", func_bad_syntax_6)
BAD_COMPILER_TEST(AST, "func/bad_syntax/", func_bad_syntax_7)
BAD_COMPILER_TEST(AST, "func/bad_syntax/", func_bad_syntax_8)
BAD_COMPILER_TEST(AST, "func/bad_syntax/", func_bad_syntax_9)
BAD_COMPILER_TEST(AST, "func/bad_syntax/", func_bad_syntax_10)

#    if ON_WINDOWS
TEST(AST, Pause)
{
    system("pause");
};
#    endif

#else
#    if FUZZING == 1
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
#    else

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
#        if ON_WINDOWS
        system("pause");
#        endif
        return compile.didScriptCompile();
    }
    puts(std::string("\x1B[32mAst:\x1B[33m\n").c_str());
    const std::string ast = compile.getJsonAST();
    puts(ast.c_str());
    libOSCleanUp();
#        if ON_WINDOWS
    system("pause");
#        endif
    return 0;
}
#    endif
#endif