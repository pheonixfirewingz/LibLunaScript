// DO NOT EDIT THIS FILE! IT IS GENERATED. EDIT TEMPLATE
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
AST_BAD_COMPILER_TEST("/func/bad_syntax",func_bad_syntax_5)
AST_BAD_COMPILER_TEST("/func/bad_syntax",func_bad_syntax_3)
AST_BAD_COMPILER_TEST("/func/bad_syntax",func_bad_syntax_7)
AST_BAD_COMPILER_TEST("/func/bad_syntax",func_bad_syntax_2)
AST_BAD_COMPILER_TEST("/func/bad_syntax",func_bad_syntax_1)
AST_BAD_COMPILER_TEST("/func/bad_syntax",func_bad_syntax_4)
AST_BAD_COMPILER_TEST("/func/bad_syntax",func_bad_syntax_9)
AST_BAD_COMPILER_TEST("/func/bad_syntax",func_bad_syntax_6)
AST_BAD_COMPILER_TEST("/func/bad_syntax",func_bad_syntax_10)
AST_BAD_COMPILER_TEST("/func/bad_syntax",func_bad_syntax_8)
AST_COMPILER_TEST("/func/common",parse_main_with_no_return)
AST_COMPILER_TEST("/func/common",parse_main_with_another_function_no_call)
AST_COMPILER_TEST("/func/common",parse_main_func)
AST_COMPILER_TEST("/func/common",parse_main_with_no_return_used_return)
AST_COMPILER_TEST("/func/common",parse_main_with_another_function_call_with_arg)
AST_COMPILER_TEST("/func/common",parse_main_with_another_function_call)
AST_COMPILER_TEST("/func/var/no_value",parse_main_func_with_float64_with_no_value)
AST_COMPILER_TEST("/func/var/no_value",parse_main_func_with_float32_with_no_value)
AST_COMPILER_TEST("/func/var/no_value",parse_main_func_with_int16_with_no_value)
AST_COMPILER_TEST("/func/var/no_value",parse_main_func_with_int64_with_no_value)
AST_COMPILER_TEST("/func/var/no_value",parse_main_func_with_uint8_with_no_value)
AST_COMPILER_TEST("/func/var/no_value",parse_main_func_with_uint32_with_no_value)
AST_COMPILER_TEST("/func/var/no_value",parse_main_func_with_int8_with_no_value)
AST_COMPILER_TEST("/func/var/no_value",parse_main_func_with_int32_with_no_value)
AST_COMPILER_TEST("/func/var/no_value",parse_main_func_with_uint64_with_no_value)
AST_COMPILER_TEST("/func/var/no_value",parse_main_func_with_uint16_with_no_value)
AST_COMPILER_TEST("/func/var/binary",parse_main_func_with_binary_expr_5)
AST_COMPILER_TEST("/func/var/binary",parse_main_func_with_binary_expr_6)
AST_COMPILER_TEST("/func/var/binary",parse_main_func_with_binary_expr_2)
AST_COMPILER_TEST("/func/var/binary",parse_main_func_with_binary_expr_3)
AST_COMPILER_TEST("/func/var/binary",parse_main_func_with_binary_expr_7)
AST_COMPILER_TEST("/func/var/binary",parse_main_func_with_binary_expr_4)
AST_COMPILER_TEST("/func/var/binary",parse_main_func_with_binary_expr_8)
AST_COMPILER_TEST("/func/var/binary",parse_main_func_with_binary_expr_1)
AST_COMPILER_TEST("/func/var/value",parse_main_func_with_int32_with_value)
AST_COMPILER_TEST("/func/var/value",parse_main_func_with_uint32_with_value)
AST_COMPILER_TEST("/func/var/value",parse_main_func_with_float32_with_value)
AST_COMPILER_TEST("/func/var/value",parse_main_func_with_uint8_with_value)
AST_COMPILER_TEST("/func/var/value",parse_main_func_with_int16_with_value)
AST_COMPILER_TEST("/func/var/value",parse_main_func_with_int8_with_value)
AST_COMPILER_TEST("/func/var/value",parse_main_func_with_uint64_with_value)
AST_COMPILER_TEST("/func/var/value",parse_main_func_with_float64_with_value)
AST_COMPILER_TEST("/func/var/value",parse_main_func_with_uint16_with_value)
AST_COMPILER_TEST("/func/var/value",parse_main_func_with_int64_with_value)
AST_COMPILER_TEST("/module",module_alone)
AST_COMPILER_TEST("/module",module_import_test_base)
AST_COMPILER_TEST("/var/binary",parse_var_binary_expr_5)
AST_COMPILER_TEST("/var/binary",parse_var_binary_expr_14)
AST_COMPILER_TEST("/var/binary",parse_var_binary_expr_2)
AST_COMPILER_TEST("/var/binary",parse_var_binary_expr_12)
AST_COMPILER_TEST("/var/binary",parse_var_binary_expr_11)
AST_COMPILER_TEST("/var/binary",parse_var_binary_expr_10)
AST_COMPILER_TEST("/var/binary",parse_var_binary_expr_9)
AST_COMPILER_TEST("/var/binary",parse_var_binary_expr_17)
AST_COMPILER_TEST("/var/binary",parse_var_binary_expr_16)
AST_COMPILER_TEST("/var/binary",parse_var_binary_expr_6)
AST_COMPILER_TEST("/var/binary",parse_var_binary_expr_8)
AST_COMPILER_TEST("/var/binary",parse_var_binary_expr_3)
AST_COMPILER_TEST("/var/binary",parse_var_binary_expr_1)
AST_COMPILER_TEST("/var/binary",parse_var_binary_expr_18)
AST_COMPILER_TEST("/var/binary",parse_var_binary_expr_15)
AST_COMPILER_TEST("/var/binary",parse_var_binary_expr_7)
AST_COMPILER_TEST("/var/binary",parse_var_binary_expr_4)
AST_COMPILER_TEST("/var/binary",parse_var_binary_expr_13)
AST_COMPILER_TEST("/var/non_global/no_value",parse_var_uint64_no_value_gn)
AST_COMPILER_TEST("/var/non_global/no_value",parse_var_float32_no_value_gn)
AST_COMPILER_TEST("/var/non_global/no_value",parse_var_int8_no_value_gn)
AST_COMPILER_TEST("/var/non_global/no_value",parse_var_int32_no_value_gn)
AST_COMPILER_TEST("/var/non_global/no_value",parse_var_float64_no_value_gn)
AST_COMPILER_TEST("/var/non_global/no_value",parse_var_uint32_no_value_gn)
AST_COMPILER_TEST("/var/non_global/no_value",parse_var_int64_no_value_gn)
AST_COMPILER_TEST("/var/non_global/no_value",parse_var_int16_no_value_gn)
AST_COMPILER_TEST("/var/non_global/no_value",parse_var_uint16_no_value_gn)
AST_COMPILER_TEST("/var/non_global/no_value",parse_var_uint8_no_value_gn)
AST_COMPILER_TEST("/var/non_global/value",parse_var_int16_with_value_gn)
AST_COMPILER_TEST("/var/non_global/value",parse_var_uint32_with_value_gn)
AST_COMPILER_TEST("/var/non_global/value",parse_var_int32_with_value_gn)
AST_COMPILER_TEST("/var/non_global/value",parse_var_uint8_with_value_gn)
AST_COMPILER_TEST("/var/non_global/value",parse_var_uint64_with_value_gn)
AST_COMPILER_TEST("/var/non_global/value",parse_var_float64_with_value_gn)
AST_COMPILER_TEST("/var/non_global/value",parse_var_int8_with_value_gn)
AST_COMPILER_TEST("/var/non_global/value",parse_var_float32_with_value_gn)
AST_COMPILER_TEST("/var/non_global/value",parse_var_int64_with_value_gn)
AST_COMPILER_TEST("/var/non_global/value",parse_var_uint16_with_value_gn)
AST_BAD_COMPILER_TEST("/var/bad_syntax",bad_syntax_1)
AST_BAD_COMPILER_TEST("/var/bad_syntax",bad_syntax_5)
AST_BAD_COMPILER_TEST("/var/bad_syntax",bad_syntax_9)
AST_BAD_COMPILER_TEST("/var/bad_syntax",bad_syntax_13)
AST_BAD_COMPILER_TEST("/var/bad_syntax",bad_syntax_11)
AST_BAD_COMPILER_TEST("/var/bad_syntax",bad_syntax_6)
AST_BAD_COMPILER_TEST("/var/bad_syntax",bad_syntax_15)
AST_BAD_COMPILER_TEST("/var/bad_syntax",bad_syntax_4)
AST_BAD_COMPILER_TEST("/var/bad_syntax",bad_syntax_14)
AST_BAD_COMPILER_TEST("/var/bad_syntax",bad_syntax_3)
AST_BAD_COMPILER_TEST("/var/bad_syntax",bad_syntax_8)
AST_BAD_COMPILER_TEST("/var/bad_syntax",bad_syntax_2)
AST_BAD_COMPILER_TEST("/var/bad_syntax",bad_syntax_12)
AST_BAD_COMPILER_TEST("/var/bad_syntax",bad_syntax_7)
AST_BAD_COMPILER_TEST("/var/bad_syntax",bad_syntax_16)
AST_BAD_COMPILER_TEST("/var/bad_syntax",bad_syntax_10)
AST_COMPILER_TEST("/var/globals/no_value",parse_var_uint8_no_value_g)
AST_COMPILER_TEST("/var/globals/no_value",parse_var_float32_no_value_g)
AST_COMPILER_TEST("/var/globals/no_value",parse_var_int32_no_value_g)
AST_COMPILER_TEST("/var/globals/no_value",parse_var_uint32_no_value_g)
AST_COMPILER_TEST("/var/globals/no_value",parse_var_int8_no_value_g)
AST_COMPILER_TEST("/var/globals/no_value",parse_var_uint16_no_value_g)
AST_COMPILER_TEST("/var/globals/no_value",parse_var_int64_no_value_g)
AST_COMPILER_TEST("/var/globals/no_value",parse_var_int16_no_value_g)
AST_COMPILER_TEST("/var/globals/no_value",parse_var_uint64_no_value_g)
AST_COMPILER_TEST("/var/globals/no_value",parse_var_float64_no_value_g)
AST_COMPILER_TEST("/var/globals/value",parse_var_int32_with_value_g)
AST_COMPILER_TEST("/var/globals/value",parse_var_uint8_with_value_g)
AST_COMPILER_TEST("/var/globals/value",parse_var_float32_with_value_g)
AST_COMPILER_TEST("/var/globals/value",parse_var_uint32_with_value_g)
AST_COMPILER_TEST("/var/globals/value",parse_var_int8_with_value_g)
AST_COMPILER_TEST("/var/globals/value",parse_var_float64_with_value_g)
AST_COMPILER_TEST("/var/globals/value",parse_var_int16_with_value_g)
AST_COMPILER_TEST("/var/globals/value",parse_var_int64_with_value_g)
AST_COMPILER_TEST("/var/globals/value",parse_var_uint64_with_value_g)
AST_COMPILER_TEST("/var/globals/value",parse_var_uint16_with_value_g)
