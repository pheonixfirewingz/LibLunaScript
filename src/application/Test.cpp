#include "Cmake.h"
#include "IO.hpp"
#include <gtest/gtest.h>
#include <liblunascript/Compiler.h>
#include <libos/Defines.h>
#include <string>

#define COMPILER_TEST(group, extend, test_name)                                                                 \
    TEST(group, test_name)                                                                                      \
    {                                                                                                           \
        libOSInit();                                                                                            \
        Compiler compiler;                                                                                      \
        setRoot(PROJECT_SOURCE_DIR);                                                                            \
        char *read_str_src;                                                                                     \
        data_size_t read_str_src_size = 0;                                                                      \
        EXPECT_TRUE(fileRead(createP(std::string("tests/src/") + extend, #test_name), &read_str_src,            \
                             &read_str_src_size) == LOS_SUCCESS);                                               \
        char *result_read;                                                                                      \
        data_size_t result_read_size = 0;                                                                       \
        EXPECT_TRUE(fileRead(createP(std::string("tests/ast/") + extend, #test_name, ".lls.ast"), &result_read, \
                             &result_read_size) == LOS_SUCCESS);                                                \
        EXPECT_FALSE(compile(&compiler, read_str_src, read_str_src_size, #test_name, strlen(#test_name)) !=     \
                     LOS_SUCCESS);                                                                              \
        std::string result(result_read, 0, result_read_size);                                                   \
        char *err_str;                                                                                          \
        data_size_t err_size = 0;                                                                               \
        astToString(compiler, &err_str, &err_size);                                                             \
        EXPECT_STREQ(std::string(err_str, 0, err_size).c_str(), result.c_str());                                \
        freeCompiler(compiler);                                                                                 \
        libOSCleanUp();                                                                                         \
    }
// fileWrite<char>(createP(std::string("tests/ast/") + extend, #test_name, ".lls.ast"), err_str, err_size);

#define BAD_COMPILER_TEST(group, extend, test_name)                                                        \
    TEST(group, test_name)                                                                                 \
    {                                                                                                      \
        libOSInit();                                                                                       \
        Compiler compiler;                                                                                 \
        setRoot(PROJECT_SOURCE_DIR);                                                                       \
        char *read_str_src;                                                                                \
        data_size_t read_str_src_size = 0;                                                                 \
        EXPECT_TRUE(fileRead(createP(std::string("tests/src/") + extend, #test_name), &read_str_src,       \
                             &read_str_src_size) == LOS_SUCCESS);                                          \
        EXPECT_TRUE(compile(&compiler, read_str_src, read_str_src_size, #test_name, strlen(#test_name)) != \
                    LOS_SUCCESS);                                                                          \
        freeCompiler(compiler);                                                                            \
        libOSCleanUp();                                                                                    \
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
// // variables with binary expression
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

TEST(AST, Pause)
{
    system("pause");
};