#include "../Cmake.h"
#include "back/ByteCodeGen.h"
#include "front/Parser.h"
#include <libos/FileIO.h>
#include <string>
#define TESTS_MODE
//#define FUZZING 
#ifndef TESTS_MODE
#ifndef FUZZING
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
    const ASTRoot *root_ast = parse(std::move(std::string((char *)read_str, 0, read_str_size)));
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
    /*auto human_readable = makeHumanReadable(generateByteCode(root_ast));
    if ((res = fileWrite("$[asset_base]/test.lbc", 23, human_readable.c_str(), human_readable.size())) != LOS_SUCCESS)
        return res;*/
    return 0;
}
#else
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
    enableSoftErrors();
    std::string x((char *)Data, 0, Size);
    const ASTRoot* v = parse(std::move(x));
    delete v;
    int ret = hasErrors()? 0: -1;
    if(ret == -1)
        puts("did error correctly");
    disableSoftErrors();
    return ret; // Values other than 0 and -1 are reserved for future use.
}
#endif
#else
#    include <gtest/gtest.h>
// global variables
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

TEST(AST, PARSE_VAR_UINT8_NO_VALUE)
{
    const char *source = "uint8 x";
    const char *result = "{\"type\":\"Program\",\"body\":[{\"type\":\"Expression\",\"id\":\"VariableDefinition\","
                         "\"name\":\"x\",\"declarations\":[{\"type\":\"Literal\",\"data_type\":\"uint8\",\"value\":"
                         "\"0\"}]}],\"SourceType\":\"module\"}";
    EXPECT_STREQ(toJson(parse(std::move(source)), false).c_str(), result);
    EXPECT_FALSE(hasErrors());
}

TEST(AST, PARSE_VAR_UINT16_NO_VALUE)
{
    const char *source = "uint16 x";
    const char *result = "{\"type\":\"Program\",\"body\":[{\"type\":\"Expression\",\"id\":\"VariableDefinition\","
                         "\"name\":\"x\",\"declarations\":[{\"type\":\"Literal\",\"data_type\":\"uint16\",\"value\":"
                         "\"0\"}]}],\"SourceType\":\"module\"}";
    EXPECT_STREQ(toJson(parse(std::move(source)), false).c_str(), result);
    EXPECT_FALSE(hasErrors());
}

TEST(AST, PARSE_VAR_UINT32_NO_VALUE)
{
    const char *source = "uint32 x";
    const char *result = "{\"type\":\"Program\",\"body\":[{\"type\":\"Expression\",\"id\":\"VariableDefinition\","
                         "\"name\":\"x\",\"declarations\":[{\"type\":\"Literal\",\"data_type\":\"uint32\",\"value\":"
                         "\"0\"}]}],\"SourceType\":\"module\"}";
    EXPECT_STREQ(toJson(parse(std::move(source)), false).c_str(), result);
    EXPECT_FALSE(hasErrors());
}

TEST(AST, PARSE_VAR_UINT64_NO_VALUE)
{
    const char *source = "uint64 x";
    const char *result = "{\"type\":\"Program\",\"body\":[{\"type\":\"Expression\",\"id\":\"VariableDefinition\","
                         "\"name\":\"x\",\"declarations\":[{\"type\":\"Literal\",\"data_type\":\"uint64\",\"value\":"
                         "\"0\"}]}],\"SourceType\":\"module\"}";
    EXPECT_STREQ(toJson(parse(std::move(source)), false).c_str(), result);
    EXPECT_FALSE(hasErrors());
}

TEST(AST, PARSE_VAR_INT8_NO_VALUE)
{
    const char *source = "int8 x";
    const char *result =
        "{\"type\":\"Program\",\"body\":[{\"type\":\"Expression\",\"id\":\"VariableDefinition\",\"name\":\"x\","
        "\"declarations\":[{\"type\":\"Literal\",\"data_type\":\"int8\",\"value\":\"0\"}]}],\"SourceType\":\"module\"}";
    EXPECT_STREQ(toJson(parse(std::move(source)), false).c_str(), result);
    EXPECT_FALSE(hasErrors());
}

TEST(AST, PARSE_VAR_INT16_NO_VALUE)
{
    const char *source = "int16 x";
    const char *result = "{\"type\":\"Program\",\"body\":[{\"type\":\"Expression\",\"id\":\"VariableDefinition\","
                         "\"name\":\"x\",\"declarations\":[{\"type\":\"Literal\",\"data_type\":\"int16\",\"value\":"
                         "\"0\"}]}],\"SourceType\":\"module\"}";
    EXPECT_STREQ(toJson(parse(std::move(source)), false).c_str(), result);
    EXPECT_FALSE(hasErrors());
}

TEST(AST, PARSE_VAR_INT32_NO_VALUE)
{
    const char *source = "int32 x";
    const char *result = "{\"type\":\"Program\",\"body\":[{\"type\":\"Expression\",\"id\":\"VariableDefinition\","
                         "\"name\":\"x\",\"declarations\":[{\"type\":\"Literal\",\"data_type\":\"int32\",\"value\":"
                         "\"0\"}]}],\"SourceType\":\"module\"}";
    EXPECT_STREQ(toJson(parse(std::move(source)), false).c_str(), result);
    EXPECT_FALSE(hasErrors());
}

TEST(AST, PARSE_VAR_INT64_NO_VALUE)
{
    const char *source = "int64 x";
    const char *result = "{\"type\":\"Program\",\"body\":[{\"type\":\"Expression\",\"id\":\"VariableDefinition\","
                         "\"name\":\"x\",\"declarations\":[{\"type\":\"Literal\",\"data_type\":\"int64\",\"value\":"
                         "\"0\"}]}],\"SourceType\":\"module\"}";
    EXPECT_STREQ(toJson(parse(std::move(source)), false).c_str(), result);
    EXPECT_FALSE(hasErrors());
}

TEST(AST, PARSE_VAR_FLOAT32_NO_VALUE)
{
    const char *source = "float32 x";
    const char *result = "{\"type\":\"Program\",\"body\":[{\"type\":\"Expression\",\"id\":\"VariableDefinition\","
                         "\"name\":\"x\",\"declarations\":[{\"type\":\"Literal\",\"data_type\":\"float32\",\"value\":"
                         "\"0.0\"}]}],\"SourceType\":\"module\"}";
    EXPECT_STREQ(toJson(parse(std::move(source)), false).c_str(), result);
    EXPECT_FALSE(hasErrors());
}

TEST(AST, PARSE_VAR_FLOAT64_NO_VALUE)
{
    const char *source = "float64 x";
    const char *result = "{\"type\":\"Program\",\"body\":[{\"type\":\"Expression\",\"id\":\"VariableDefinition\","
                         "\"name\":\"x\",\"declarations\":[{\"type\":\"Literal\",\"data_type\":\"float64\",\"value\":"
                         "\"0.0\"}]}],\"SourceType\":\"module\"}";
    EXPECT_STREQ(toJson(parse(std::move(source)), false).c_str(), result);
    EXPECT_FALSE(hasErrors());
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
TEST(AST, PARSE_VAR_UINT8_WITH_VALUE)
{
    const char *source = "uint8 x = 20";
    const char *result = "{\"type\":\"Program\",\"body\":[{\"type\":\"Expression\",\"id\":\"VariableDefinition\","
                         "\"name\":\"x\",\"declarations\":[{\"type\":\"Literal\",\"data_type\":\"uint8\",\"value\":"
                         "\"20\"}]}],\"SourceType\":\"module\"}";
    EXPECT_STREQ(toJson(parse(std::move(source)), false).c_str(), result);
    EXPECT_FALSE(hasErrors());
}

TEST(AST, PARSE_VAR_UINT16_WITH_VALUE)
{
    const char *source = "uint16 x = 20";
    const char *result = "{\"type\":\"Program\",\"body\":[{\"type\":\"Expression\",\"id\":\"VariableDefinition\","
                         "\"name\":\"x\",\"declarations\":[{\"type\":\"Literal\",\"data_type\":\"uint16\",\"value\":"
                         "\"20\"}]}],\"SourceType\":\"module\"}";
    EXPECT_STREQ(toJson(parse(std::move(source)), false).c_str(), result);
    EXPECT_FALSE(hasErrors());
}

TEST(AST, PARSE_VAR_UINT32_WITH_VALUE)
{
    const char *source = "uint32 x = 20";
    const char *result = "{\"type\":\"Program\",\"body\":[{\"type\":\"Expression\",\"id\":\"VariableDefinition\","
                         "\"name\":\"x\",\"declarations\":[{\"type\":\"Literal\",\"data_type\":\"uint32\",\"value\":"
                         "\"20\"}]}],\"SourceType\":\"module\"}";
    EXPECT_STREQ(toJson(parse(std::move(source)), false).c_str(), result);
    EXPECT_FALSE(hasErrors());
}

TEST(AST, PARSE_VAR_UINT64_WITH_VALUE)
{
    const char *source = "uint64 x = 20";
    const char *result = "{\"type\":\"Program\",\"body\":[{\"type\":\"Expression\",\"id\":\"VariableDefinition\","
                         "\"name\":\"x\",\"declarations\":[{\"type\":\"Literal\",\"data_type\":\"uint64\",\"value\":"
                         "\"20\"}]}],\"SourceType\":\"module\"}";
    EXPECT_STREQ(toJson(parse(std::move(source)), false).c_str(), result);
    EXPECT_FALSE(hasErrors());
}

TEST(AST, PARSE_VAR_INT8_WITH_VALUE)
{
    const char *source = "int8 x = 20";
    const char *result = "{\"type\":\"Program\",\"body\":[{\"type\":\"Expression\",\"id\":\"VariableDefinition\","
                         "\"name\":\"x\",\"declarations\":[{\"type\":\"Literal\",\"data_type\":\"int8\",\"value\":"
                         "\"20\"}]}],\"SourceType\":\"module\"}";
    EXPECT_STREQ(toJson(parse(std::move(source)), false).c_str(), result);
    EXPECT_FALSE(hasErrors());
}

TEST(AST, PARSE_VAR_INT16_WITH_VALUE)
{
    const char *source = "int16 x = 20";
    const char *result = "{\"type\":\"Program\",\"body\":[{\"type\":\"Expression\",\"id\":\"VariableDefinition\","
                         "\"name\":\"x\",\"declarations\":[{\"type\":\"Literal\",\"data_type\":\"int16\",\"value\":"
                         "\"20\"}]}],\"SourceType\":\"module\"}";
    EXPECT_STREQ(toJson(parse(std::move(source)), false).c_str(), result);
    EXPECT_FALSE(hasErrors());
}

TEST(AST, PARSE_VAR_INT32_WITH_VALUE)
{
    const char *source = "int32 x = 20";
    const char *result = "{\"type\":\"Program\",\"body\":[{\"type\":\"Expression\",\"id\":\"VariableDefinition\","
                         "\"name\":\"x\",\"declarations\":[{\"type\":\"Literal\",\"data_type\":\"int32\",\"value\":"
                         "\"20\"}]}],\"SourceType\":\"module\"}";
    EXPECT_STREQ(toJson(parse(std::move(source)), false).c_str(), result);
    EXPECT_FALSE(hasErrors());
}

TEST(AST, PARSE_VAR_INT64_WITH_VALUE)
{
    const char *source = "int64 x = 20";
    const char *result = "{\"type\":\"Program\",\"body\":[{\"type\":\"Expression\",\"id\":\"VariableDefinition\","
                         "\"name\":\"x\",\"declarations\":[{\"type\":\"Literal\",\"data_type\":\"int64\",\"value\":"
                         "\"20\"}]}],\"SourceType\":\"module\"}";
    EXPECT_STREQ(toJson(parse(std::move(source)), false).c_str(), result);
    EXPECT_FALSE(hasErrors());
}

TEST(AST, PARSE_VAR_FLOAT32_WITH_VALUE)
{
    const char *source = "float32 x = 20.0";
    const char *result = "{\"type\":\"Program\",\"body\":[{\"type\":\"Expression\",\"id\":\"VariableDefinition\","
                         "\"name\":\"x\",\"declarations\":[{\"type\":\"Literal\",\"data_type\":\"float32\",\"value\":"
                         "\"20.0\"}]}],\"SourceType\":\"module\"}";
    EXPECT_STREQ(toJson(parse(std::move(source)), false).c_str(), result);
    EXPECT_FALSE(hasErrors());
}

TEST(AST, PARSE_VAR_FLOAT64_WITH_VALUE)
{
    const char *source = "float64 x = 20.0";
    const char *result = "{\"type\":\"Program\",\"body\":[{\"type\":\"Expression\",\"id\":\"VariableDefinition\","
                         "\"name\":\"x\",\"declarations\":[{\"type\":\"Literal\",\"data_type\":\"float64\",\"value\":"
                         "\"20.0\"}]}],\"SourceType\":\"module\"}";
    EXPECT_STREQ(toJson(parse(std::move(source)), false).c_str(), result);
    EXPECT_FALSE(hasErrors());
}

TEST(AST, PARSE_VAR_WITH_BINARY_EXPR_1)
{
    const char *source = "float64 x = 20.0 + 35.0";
    const char *result = "{\"type\":\"Program\",\"body\":[{\"type\":\"Expression\",\"id\":\"VariableDefinition\","
                         "\"name\":\"x\",\"declarations\":[{\"type\":\"BinaryExpression\",\"precedence\":5,\"op\":\"+"
                         "\",\"right\":{\"type\":\"Literal\",\"data_type\":\"any "
                         "float\",\"value\":\"35.0\"},\"left\":{\"type\":\"Literal\",\"data_type\":\"any "
                         "float\",\"value\":\"20.0\"}}]}],\"SourceType\":\"module\"}";
    EXPECT_STREQ(toJson(parse(std::move(source)), false).c_str(), result);
    EXPECT_FALSE(hasErrors());
}

TEST(AST, PARSE_VAR_WITH_BINARY_EXPR_2)
{
    const char *source = "int64 x = 20 + 35";
    const char *result =
        "{\"type\":\"Program\",\"body\":[{\"type\":\"Expression\",\"id\":\"VariableDefinition\",\"name\":\"x\","
        "\"declarations\":[{\"type\":\"BinaryExpression\",\"precedence\":5,\"op\":\"+\",\"right\":{\"type\":"
        "\"Literal\",\"data_type\":\"any uint\",\"value\":\"35\"},\"left\":{\"type\":\"Literal\",\"data_type\":\"any "
        "uint\",\"value\":\"20\"}}]}],\"SourceType\":\"module\"}";
    EXPECT_STREQ(toJson(parse(std::move(source)), false).c_str(), result);
    EXPECT_FALSE(hasErrors());
}

TEST(AST, PARSE_VAR_WITH_BINARY_EXPR_3)
{
    const char *source = "float64 x = 20.0 / 35.0";
    const char *result = "{\"type\":\"Program\",\"body\":[{\"type\":\"Expression\",\"id\":\"VariableDefinition\","
                         "\"name\":\"x\",\"declarations\":[{\"type\":\"BinaryExpression\",\"precedence\":6,\"op\":\"/"
                         "\",\"right\":{\"type\":\"Literal\",\"data_type\":\"any "
                         "float\",\"value\":\"35.0\"},\"left\":{\"type\":\"Literal\",\"data_type\":\"any "
                         "float\",\"value\":\"20.0\"}}]}],\"SourceType\":\"module\"}";
    EXPECT_STREQ(toJson(parse(std::move(source)), false).c_str(), result);
    EXPECT_FALSE(hasErrors());
}

TEST(AST, PARSE_VAR_WITH_BINARY_EXPR_4)
{
    const char *source = "int64 x = 20 / 35";
    const char *result = "{\"type\":\"Program\",\"body\":[{\"type\":\"Expression\",\"id\":\"VariableDefinition\","
                         "\"name\":\"x\",\"declarations\":[{\"type\":\"BinaryExpression\",\"precedence\":6,\"op\":\"/"
                         "\",\"right\":{\"type\":\"Literal\",\"data_type\":\"any "
                         "uint\",\"value\":\"35\"},\"left\":{\"type\":\"Literal\",\"data_type\":\"any "
                         "uint\",\"value\":\"20\"}}]}],\"SourceType\":\"module\"}";
    EXPECT_STREQ(toJson(parse(std::move(source)), false).c_str(), result);
    EXPECT_FALSE(hasErrors());
}

TEST(AST, PARSE_VAR_WITH_BINARY_EXPR_5)
{
    const char *source = "float64 x = 20.0 * 35.0";
    const char *result = "{\"type\":\"Program\",\"body\":[{\"type\":\"Expression\",\"id\":\"VariableDefinition\","
                         "\"name\":\"x\",\"declarations\":[{\"type\":\"BinaryExpression\",\"precedence\":6,\"op\":\"*"
                         "\",\"right\":{\"type\":\"Literal\",\"data_type\":\"any "
                         "float\",\"value\":\"35.0\"},\"left\":{\"type\":\"Literal\",\"data_type\":\"any "
                         "float\",\"value\":\"20.0\"}}]}],\"SourceType\":\"module\"}";
    EXPECT_STREQ(toJson(parse(std::move(source)), false).c_str(), result);
    EXPECT_FALSE(hasErrors());
}

TEST(AST, PARSE_VAR_WITH_BINARY_EXPR_6)
{
    const char *source = "int64 x = 20 * 35";
    const char *result =
        "{\"type\":\"Program\",\"body\":[{\"type\":\"Expression\",\"id\":\"VariableDefinition\",\"name\":\"x\","
        "\"declarations\":[{\"type\":\"BinaryExpression\",\"precedence\":6,\"op\":\"*\",\"right\":{\"type\":"
        "\"Literal\",\"data_type\":\"any uint\",\"value\":\"35\"},\"left\":{\"type\":\"Literal\",\"data_type\":\"any "
        "uint\",\"value\":\"20\"}}]}],\"SourceType\":\"module\"}";
    EXPECT_STREQ(toJson(parse(std::move(source)), false).c_str(), result);
    EXPECT_FALSE(hasErrors());
}

TEST(AST, PARSE_VAR_WITH_BINARY_EXPR_7)
{
    const char *source = "float64 x = 20.0 - 35.0";
    const char *result = "{\"type\":\"Program\",\"body\":[{\"type\":\"Expression\",\"id\":\"VariableDefinition\","
                         "\"name\":\"x\",\"declarations\":[{\"type\":\"BinaryExpression\",\"precedence\":5,\"op\":\"-"
                         "\",\"right\":{\"type\":\"Literal\",\"data_type\":\"any "
                         "float\",\"value\":\"35.0\"},\"left\":{\"type\":\"Literal\",\"data_type\":\"any "
                         "float\",\"value\":\"20.0\"}}]}],\"SourceType\":\"module\"}";
    EXPECT_STREQ(toJson(parse(std::move(source)), false).c_str(), result);
    EXPECT_FALSE(hasErrors());
}

TEST(AST, PARSE_VAR_WITH_BINARY_EXPR_8)
{
    const char *source = "int64 x = 20 - 35";
    const char *result =
        "{\"type\":\"Program\",\"body\":[{\"type\":\"Expression\",\"id\":\"VariableDefinition\",\"name\":\"x\","
        "\"declarations\":[{\"type\":\"BinaryExpression\",\"precedence\":5,\"op\":\"-\",\"right\":{\"type\":"
        "\"Literal\",\"data_type\":\"any uint\",\"value\":\"35\"},\"left\":{\"type\":\"Literal\",\"data_type\":\"any "
        "uint\",\"value\":\"20\"}}]}],\"SourceType\":\"module\"}";
    EXPECT_STREQ(toJson(parse(std::move(source)), false).c_str(), result);
    EXPECT_FALSE(hasErrors());
}

TEST(AST, PARSE_VAR_WITH_BINARY_EXPR_9)
{
    const char *source = "float64 x = 20.0 or 35.0";
    const char *result = "{\"type\":\"Program\",\"body\":[{\"type\":\"Expression\",\"id\":\"VariableDefinition\","
                         "\"name\":\"x\",\"declarations\":[{\"type\":\"BinaryExpression\",\"precedence\":1,\"op\":"
                         "\"or\",\"right\":{\"type\":\"Literal\",\"data_type\":\"any "
                         "float\",\"value\":\"35.0\"},\"left\":{\"type\":\"Literal\",\"data_type\":\"any "
                         "float\",\"value\":\"20.0\"}}]}],\"SourceType\":\"module\"}";
    EXPECT_STREQ(toJson(parse(std::move(source)), false).c_str(), result);
    EXPECT_FALSE(hasErrors());
}

TEST(AST, PARSE_VAR_WITH_BINARY_EXPR_10)
{
    const char *source = "int64 x = 20 or 35";
    const char *result =
        "{\"type\":\"Program\",\"body\":[{\"type\":\"Expression\",\"id\":\"VariableDefinition\",\"name\":\"x\","
        "\"declarations\":[{\"type\":\"BinaryExpression\",\"precedence\":1,\"op\":\"or\",\"right\":{\"type\":"
        "\"Literal\",\"data_type\":\"any uint\",\"value\":\"35\"},\"left\":{\"type\":\"Literal\",\"data_type\":\"any "
        "uint\",\"value\":\"20\"}}]}],\"SourceType\":\"module\"}";
    EXPECT_STREQ(toJson(parse(std::move(source)), false).c_str(), result);
    EXPECT_FALSE(hasErrors());
}

TEST(AST, PARSE_VAR_WITH_BINARY_EXPR_11)
{
    const char *source = "float64 x = 20.0 xor 35.0";
    const char *result = "{\"type\":\"Program\",\"body\":[{\"type\":\"Expression\",\"id\":\"VariableDefinition\","
                         "\"name\":\"x\",\"declarations\":[{\"type\":\"BinaryExpression\",\"precedence\":1,\"op\":"
                         "\"xor\",\"right\":{\"type\":\"Literal\",\"data_type\":\"any "
                         "float\",\"value\":\"35.0\"},\"left\":{\"type\":\"Literal\",\"data_type\":\"any "
                         "float\",\"value\":\"20.0\"}}]}],\"SourceType\":\"module\"}";
    EXPECT_STREQ(toJson(parse(std::move(source)), false).c_str(), result);
    EXPECT_FALSE(hasErrors());
}

TEST(AST, PARSE_VAR_WITH_BINARY_EXPR_12)
{
    const char *source = "int64 x = 20 xor 35";
    const char *result =
        "{\"type\":\"Program\",\"body\":[{\"type\":\"Expression\",\"id\":\"VariableDefinition\",\"name\":\"x\","
        "\"declarations\":[{\"type\":\"BinaryExpression\",\"precedence\":1,\"op\":\"xor\",\"right\":{\"type\":"
        "\"Literal\",\"data_type\":\"any uint\",\"value\":\"35\"},\"left\":{\"type\":\"Literal\",\"data_type\":\"any "
        "uint\",\"value\":\"20\"}}]}],\"SourceType\":\"module\"}";
    EXPECT_STREQ(toJson(parse(std::move(source)), false).c_str(), result);
    EXPECT_FALSE(hasErrors());
}

TEST(AST, PARSE_VAR_WITH_BINARY_EXPR_13)
{
    const char *source = "float64 x = 20.0 and 35.0";
    const char *result = "{\"type\":\"Program\",\"body\":[{\"type\":\"Expression\",\"id\":\"VariableDefinition\","
                         "\"name\":\"x\",\"declarations\":[{\"type\":\"BinaryExpression\",\"precedence\":1,\"op\":"
                         "\"and\",\"right\":{\"type\":\"Literal\",\"data_type\":\"any "
                         "float\",\"value\":\"35.0\"},\"left\":{\"type\":\"Literal\",\"data_type\":\"any "
                         "float\",\"value\":\"20.0\"}}]}],\"SourceType\":\"module\"}";
    EXPECT_STREQ(toJson(parse(std::move(source)), false).c_str(), result);
    EXPECT_FALSE(hasErrors());
}

TEST(AST, PARSE_VAR_WITH_BINARY_EXPR_14)
{
    const char *source = "int64 x = 20 and 35";
    const char *result =
        "{\"type\":\"Program\",\"body\":[{\"type\":\"Expression\",\"id\":\"VariableDefinition\",\"name\":\"x\","
        "\"declarations\":[{\"type\":\"BinaryExpression\",\"precedence\":1,\"op\":\"and\",\"right\":{\"type\":"
        "\"Literal\",\"data_type\":\"any uint\",\"value\":\"35\"},\"left\":{\"type\":\"Literal\",\"data_type\":\"any "
        "uint\",\"value\":\"20\"}}]}],\"SourceType\":\"module\"}";
    EXPECT_STREQ(toJson(parse(std::move(source)), false).c_str(), result);
    EXPECT_FALSE(hasErrors());
}

TEST(AST, PARSE_VAR_WITH_BINARY_EXPR_15)
{
    const char *source = "float64 x = 20.0 mod 35.0";
    const char *result = "{\"type\":\"Program\",\"body\":[{\"type\":\"Expression\",\"id\":\"VariableDefinition\","
                         "\"name\":\"x\",\"declarations\":[{\"type\":\"BinaryExpression\",\"precedence\":6,\"op\":"
                         "\"modulo\",\"right\":{\"type\":\"Literal\",\"data_type\":\"any "
                         "float\",\"value\":\"35.0\"},\"left\":{\"type\":\"Literal\",\"data_type\":\"any "
                         "float\",\"value\":\"20.0\"}}]}],\"SourceType\":\"module\"}";
    EXPECT_STREQ(toJson(parse(std::move(source)), false).c_str(), result);
    EXPECT_FALSE(hasErrors());
}

TEST(AST, PARSE_VAR_WITH_BINARY_EXPR_16)
{
    const char *source = "int64 x = 20 mod 35";
    const char *result =
        "{\"type\":\"Program\",\"body\":[{\"type\":\"Expression\",\"id\":\"VariableDefinition\",\"name\":\"x\","
        "\"declarations\":[{\"type\":\"BinaryExpression\",\"precedence\":6,\"op\":\"modulo\",\"right\":{\"type\":"
        "\"Literal\",\"data_type\":\"any uint\",\"value\":\"35\"},\"left\":{\"type\":\"Literal\",\"data_type\":\"any "
        "uint\",\"value\":\"20\"}}]}],\"SourceType\":\"module\"}";
    EXPECT_STREQ(toJson(parse(std::move(source)), false).c_str(), result);
    EXPECT_FALSE(hasErrors());
}

TEST(AST, PARSE_VAR_WITH_BINARY_EXPR_17)
{
    const char *source = "float64 x = 20.0 + 35.0 - 100.0 / 6.0 and 8.0";
    const char *result =
        "{\"type\":\"Program\",\"body\":[{\"type\":\"Expression\",\"id\":\"VariableDefinition\",\"name\":\"x\","
        "\"declarations\":[{\"type\":\"BinaryExpression\",\"precedence\":5,\"op\":\"+\",\"right\":{\"type\":"
        "\"BinaryExpression\",\"precedence\":6,\"op\":\"-\",\"right\":{\"type\":\"BinaryExpression\",\"precedence\":8,"
        "\"op\":\"/"
        "\",\"right\":{\"type\":\"BinaryExpression\",\"precedence\":4,\"op\":\"and\",\"right\":{\"type\":\"Literal\","
        "\"data_type\":\"any float\",\"value\":\"8.0\"},\"left\":{\"type\":\"Literal\",\"data_type\":\"any "
        "float\",\"value\":\"6.0\"}},\"left\":{\"type\":\"Literal\",\"data_type\":\"any "
        "float\",\"value\":\"100.0\"}},\"left\":{\"type\":\"Literal\",\"data_type\":\"any "
        "float\",\"value\":\"35.0\"}},\"left\":{\"type\":\"Literal\",\"data_type\":\"any "
        "float\",\"value\":\"20.0\"}}]}],\"SourceType\":\"module\"}";
    EXPECT_STREQ(toJson(parse(std::move(source)), false).c_str(), result);
    EXPECT_FALSE(hasErrors());
}

TEST(AST, PARSE_VAR_WITH_BINARY_EXPR_18)
{
    const char *source = "int64 x = 20 + 35 - 100 / 6 and 8";
    const char *result =
        "{\"type\":\"Program\",\"body\":[{\"type\":\"Expression\",\"id\":\"VariableDefinition\",\"name\":\"x\","
        "\"declarations\":[{\"type\":\"BinaryExpression\",\"precedence\":5,\"op\":\"+\",\"right\":{\"type\":"
        "\"BinaryExpression\",\"precedence\":6,\"op\":\"-\",\"right\":{\"type\":\"BinaryExpression\",\"precedence\":8,"
        "\"op\":\"/"
        "\",\"right\":{\"type\":\"BinaryExpression\",\"precedence\":4,\"op\":\"and\",\"right\":{\"type\":\"Literal\","
        "\"data_type\":\"any uint\",\"value\":\"8\"},\"left\":{\"type\":\"Literal\",\"data_type\":\"any "
        "uint\",\"value\":\"6\"}},\"left\":{\"type\":\"Literal\",\"data_type\":\"any "
        "uint\",\"value\":\"100\"}},\"left\":{\"type\":\"Literal\",\"data_type\":\"any "
        "uint\",\"value\":\"35\"}},\"left\":{\"type\":\"Literal\",\"data_type\":\"any "
        "uint\",\"value\":\"20\"}}]}],\"SourceType\":\"module\"}";
    EXPECT_STREQ(toJson(parse(std::move(source)), false).c_str(), result);
    EXPECT_FALSE(hasErrors());
}

// function return tests
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

TEST(AST, PARSE_MAIN)
{
    const char *source = "func main() -> int32 { ret 0}";
    const char *result = "{\"type\":\"Program\",\"body\":[{\"type\":\"FunctionDef\",\"id\":\"main\",\"return_type\":"
                         "\"int32\",\"parameters\""
                         ":{\"type\":\"Expression\",\"id\":\"ParameterList\",\"declarations\":[]},\"body\":{\"type\":"
                         "\"ExecutionBlock\",\"execution\""
                         ":[{\"type\":\"Expression\",\"id\":\"ReturnType\",\"declarations\":[{\"type\":\"Literal\","
                         "\"data_type\":\"To Be Determined\""
                         ",\"value\":\"0\"}]}]}}],\"SourceType\":\"module\"}";
    EXPECT_STREQ(toJson(parse(std::move(source)), false).c_str(), result);
    EXPECT_FALSE(hasErrors());
}

TEST(AST, PARSE_MAIN_WITH_VAR_UINT8_WITH_VALUE)
{
    const char *source = "func main() -> int32 { uint8 x = 30 ret 0}";
    const char *result =
        "{\"type\":\"Program\",\"body\":[{\"type\":\"FunctionDef\",\"id\":\"main\",\"return_type\":\"int32\","
        "\"parameters\":{\"type\":\"Expression\",\"id\":\"ParameterList\",\"declarations\":[]},\"body\":{\"type\":"
        "\"ExecutionBlock\",\"execution\":[{\"type\":\"Expression\",\"id\":\"VariableDefinition\",\"name\":\"x\","
        "\"declarations\":[{\"type\":\"Literal\",\"data_type\":\"uint8\",\"value\":\"30\"}]},{\"type\":"
        "\"Expression\","
        "\"id\":\"ReturnType\",\"declarations\":[{\"type\":\"Literal\",\"data_type\":\"To Be "
        "Determined\",\"value\":\"0\"}]}]}}],\"SourceType\":\"module\"}";
    EXPECT_STREQ(toJson(parse(std::move(source)), false).c_str(), result);
    EXPECT_FALSE(hasErrors());
}

TEST(AST, PARSE_MAIN_WITH_VAR_UINT16_WITH_VALUE)
{
    const char *source = "func main() -> int32 { uint16 x = 30 ret 0}";
    const char *result =
        "{\"type\":\"Program\",\"body\":[{\"type\":\"FunctionDef\",\"id\":\"main\",\"return_type\":\"int32\","
        "\"parameters\":{\"type\":\"Expression\",\"id\":\"ParameterList\",\"declarations\":[]},\"body\":{\"type\":"
        "\"ExecutionBlock\",\"execution\":[{\"type\":\"Expression\",\"id\":\"VariableDefinition\",\"name\":\"x\","
        "\"declarations\":[{\"type\":\"Literal\",\"data_type\":\"uint16\",\"value\":\"30\"}]},{\"type\":"
        "\"Expression\","
        "\"id\":\"ReturnType\",\"declarations\":[{\"type\":\"Literal\",\"data_type\":\"To Be "
        "Determined\",\"value\":\"0\"}]}]}}],\"SourceType\":\"module\"}";
    EXPECT_STREQ(toJson(parse(std::move(source)), false).c_str(), result);
    EXPECT_FALSE(hasErrors());
}

TEST(AST, PARSE_MAIN_WITH_VAR_UINT32_WITH_VALUE)
{
    const char *source = "func main() -> int32 { uint32 x = 30 ret 0}";
    const char *result =
        "{\"type\":\"Program\",\"body\":[{\"type\":\"FunctionDef\",\"id\":\"main\",\"return_type\":\"int32\","
        "\"parameters\":{\"type\":\"Expression\",\"id\":\"ParameterList\",\"declarations\":[]},\"body\":{\"type\":"
        "\"ExecutionBlock\",\"execution\":[{\"type\":\"Expression\",\"id\":\"VariableDefinition\",\"name\":\"x\","
        "\"declarations\":[{\"type\":\"Literal\",\"data_type\":\"uint32\",\"value\":\"30\"}]},{\"type\":"
        "\"Expression\","
        "\"id\":\"ReturnType\",\"declarations\":[{\"type\":\"Literal\",\"data_type\":\"To Be "
        "Determined\",\"value\":\"0\"}]}]}}],\"SourceType\":\"module\"}";
    EXPECT_STREQ(toJson(parse(std::move(source)), false).c_str(), result);
    EXPECT_FALSE(hasErrors());
}

TEST(AST, PARSE_MAIN_WITH_VAR_UINT64_WITH_VALUE)
{
    const char *source = "func main() -> int32 { uint64 x = 30 ret 0}";
    const char *result =
        "{\"type\":\"Program\",\"body\":[{\"type\":\"FunctionDef\",\"id\":\"main\",\"return_type\":\"int32\","
        "\"parameters\":{\"type\":\"Expression\",\"id\":\"ParameterList\",\"declarations\":[]},\"body\":{\"type\":"
        "\"ExecutionBlock\",\"execution\":[{\"type\":\"Expression\",\"id\":\"VariableDefinition\",\"name\":\"x\","
        "\"declarations\":[{\"type\":\"Literal\",\"data_type\":\"uint64\",\"value\":\"30\"}]},{\"type\":"
        "\"Expression\","
        "\"id\":\"ReturnType\",\"declarations\":[{\"type\":\"Literal\",\"data_type\":\"To Be "
        "Determined\",\"value\":\"0\"}]}]}}],\"SourceType\":\"module\"}";
    EXPECT_STREQ(toJson(parse(std::move(source)), false).c_str(), result);
    EXPECT_FALSE(hasErrors());
}

TEST(AST, PARSE_MAIN_WITH_VAR_INT8_WITH_VALUE)
{
    const char *source = "func main() -> int32 { int8 x = 30 ret 0}";
    const char *result =
        "{\"type\":\"Program\",\"body\":[{\"type\":\"FunctionDef\",\"id\":\"main\",\"return_type\":\"int32\","
        "\"parameters\":{\"type\":\"Expression\",\"id\":\"ParameterList\",\"declarations\":[]},\"body\":{\"type\":"
        "\"ExecutionBlock\",\"execution\":[{\"type\":\"Expression\",\"id\":\"VariableDefinition\",\"name\":\"x\","
        "\"declarations\":[{\"type\":\"Literal\",\"data_type\":\"int8\",\"value\":\"30\"}]},{\"type\":"
        "\"Expression\","
        "\"id\":\"ReturnType\",\"declarations\":[{\"type\":\"Literal\",\"data_type\":\"To Be "
        "Determined\",\"value\":\"0\"}]}]}}],\"SourceType\":\"module\"}";
    EXPECT_STREQ(toJson(parse(std::move(source)), false).c_str(), result);
    EXPECT_FALSE(hasErrors());
}

TEST(AST, PARSE_MAIN_WITH_VAR_INT16_WITH_VALUE)
{
    const char *source = "func main() -> int32 { int16 x = 30 ret 0}";
    const char *result =
        "{\"type\":\"Program\",\"body\":[{\"type\":\"FunctionDef\",\"id\":\"main\",\"return_type\":\"int32\","
        "\"parameters\":{\"type\":\"Expression\",\"id\":\"ParameterList\",\"declarations\":[]},\"body\":{\"type\":"
        "\"ExecutionBlock\",\"execution\":[{\"type\":\"Expression\",\"id\":\"VariableDefinition\",\"name\":\"x\","
        "\"declarations\":[{\"type\":\"Literal\",\"data_type\":\"int16\",\"value\":\"30\"}]},{\"type\":"
        "\"Expression\","
        "\"id\":\"ReturnType\",\"declarations\":[{\"type\":\"Literal\",\"data_type\":\"To Be "
        "Determined\",\"value\":\"0\"}]}]}}],\"SourceType\":\"module\"}";
    EXPECT_STREQ(toJson(parse(std::move(source)), false).c_str(), result);
    EXPECT_FALSE(hasErrors());
}

TEST(AST, PARSE_MAIN_WITH_VAR_INT32_WITH_VALUE)
{
    const char *source = "func main() -> int32 { int32 x = 30 ret 0}";
    const char *result =
        "{\"type\":\"Program\",\"body\":[{\"type\":\"FunctionDef\",\"id\":\"main\",\"return_type\":\"int32\","
        "\"parameters\":{\"type\":\"Expression\",\"id\":\"ParameterList\",\"declarations\":[]},\"body\":{\"type\":"
        "\"ExecutionBlock\",\"execution\":[{\"type\":\"Expression\",\"id\":\"VariableDefinition\",\"name\":\"x\","
        "\"declarations\":[{\"type\":\"Literal\",\"data_type\":\"int32\",\"value\":\"30\"}]},{\"type\":"
        "\"Expression\","
        "\"id\":\"ReturnType\",\"declarations\":[{\"type\":\"Literal\",\"data_type\":\"To Be "
        "Determined\",\"value\":\"0\"}]}]}}],\"SourceType\":\"module\"}";
    EXPECT_STREQ(toJson(parse(std::move(source)), false).c_str(), result);
    EXPECT_FALSE(hasErrors());
}

TEST(AST, PARSE_MAIN_WITH_VAR_INT64_WITH_VALUE)
{
    const char *source = "func main() -> int32 { int64 x = 30 ret 0}";
    const char *result =
        "{\"type\":\"Program\",\"body\":[{\"type\":\"FunctionDef\",\"id\":\"main\",\"return_type\":\"int32\","
        "\"parameters\":{\"type\":\"Expression\",\"id\":\"ParameterList\",\"declarations\":[]},\"body\":{\"type\":"
        "\"ExecutionBlock\",\"execution\":[{\"type\":\"Expression\",\"id\":\"VariableDefinition\",\"name\":\"x\","
        "\"declarations\":[{\"type\":\"Literal\",\"data_type\":\"int64\",\"value\":\"30\"}]},{\"type\":"
        "\"Expression\","
        "\"id\":\"ReturnType\",\"declarations\":[{\"type\":\"Literal\",\"data_type\":\"To Be "
        "Determined\",\"value\":\"0\"}]}]}}],\"SourceType\":\"module\"}";
    EXPECT_STREQ(toJson(parse(std::move(source)), false).c_str(), result);
    EXPECT_FALSE(hasErrors());
}

TEST(AST, PARSE_MAIN_WITH_VAR_FLOAT32_WITH_VALUE)
{
    const char *source = "func main() -> int32 { float32 x = 30.0 ret 0}";
    const char *result =
        "{\"type\":\"Program\",\"body\":[{\"type\":\"FunctionDef\",\"id\":\"main\",\"return_type\":\"int32\","
        "\"parameters\":{\"type\":\"Expression\",\"id\":\"ParameterList\",\"declarations\":[]},\"body\":{\"type\":"
        "\"ExecutionBlock\",\"execution\":[{\"type\":\"Expression\",\"id\":\"VariableDefinition\",\"name\":\"x\","
        "\"declarations\":[{\"type\":\"Literal\",\"data_type\":\"float32\",\"value\":\"30.0\"}]},{\"type\":"
        "\"Expression\",\"id\":\"ReturnType\",\"declarations\":[{\"type\":\"Literal\",\"data_type\":\"To Be "
        "Determined\",\"value\":\"0\"}]}]}}],\"SourceType\":\"module\"}";
    EXPECT_STREQ(toJson(parse(std::move(source)), false).c_str(), result);
    EXPECT_FALSE(hasErrors());
}

TEST(AST, PARSE_MAIN_WITH_VAR_FLOAT64_WITH_VALUE)
{
    const char *source = "func main() -> int32 { float64 x = 30.0 ret 0}";
    const char *result =
        "{\"type\":\"Program\",\"body\":[{\"type\":\"FunctionDef\",\"id\":\"main\",\"return_type\":\"int32\","
        "\"parameters\":{\"type\":\"Expression\",\"id\":\"ParameterList\",\"declarations\":[]},\"body\":{\"type\":"
        "\"ExecutionBlock\",\"execution\":[{\"type\":\"Expression\",\"id\":\"VariableDefinition\",\"name\":\"x\","
        "\"declarations\":[{\"type\":\"Literal\",\"data_type\":\"float64\",\"value\":\"30.0\"}]},{\"type\":"
        "\"Expression\",\"id\":\"ReturnType\",\"declarations\":[{\"type\":\"Literal\",\"data_type\":\"To Be "
        "Determined\",\"value\":\"0\"}]}]}}],\"SourceType\":\"module\"}";
    EXPECT_STREQ(toJson(parse(std::move(source)), false).c_str(), result);
    EXPECT_FALSE(hasErrors());
}

TEST(AST, PARSE_MAIN_WITH_VAR_UINT8_WITHOUT_VALUE)
{
    const char *source = "func main() -> int32 { uint8 x ret 0}";
    const char *result =
        "{\"type\":\"Program\",\"body\":[{\"type\":\"FunctionDef\",\"id\":\"main\",\"return_type\":\"int32\","
        "\"parameters\":{\"type\":\"Expression\",\"id\":\"ParameterList\",\"declarations\":[]},\"body\":{\"type\":"
        "\"ExecutionBlock\",\"execution\":[{\"type\":\"Expression\",\"id\":\"VariableDefinition\",\"name\":\"x\","
        "\"declarations\":[{\"type\":\"Literal\",\"data_type\":\"uint8\",\"value\":\"0\"}]},{\"type\":"
        "\"Expression\","
        "\"id\":\"ReturnType\",\"declarations\":[{\"type\":\"Literal\",\"data_type\":\"To Be "
        "Determined\",\"value\":\"0\"}]}]}}],\"SourceType\":\"module\"}";
    EXPECT_STREQ(toJson(parse(std::move(source)), false).c_str(), result);
    EXPECT_FALSE(hasErrors());
}

TEST(AST, PARSE_MAIN_WITH_VAR_UINT16_WITHOUT_VALUE)
{
    const char *source = "func main() -> int32 { uint16 x ret 0}";
    const char *result =
        "{\"type\":\"Program\",\"body\":[{\"type\":\"FunctionDef\",\"id\":\"main\",\"return_type\":\"int32\","
        "\"parameters\":{\"type\":\"Expression\",\"id\":\"ParameterList\",\"declarations\":[]},\"body\":{\"type\":"
        "\"ExecutionBlock\",\"execution\":[{\"type\":\"Expression\",\"id\":\"VariableDefinition\",\"name\":\"x\","
        "\"declarations\":[{\"type\":\"Literal\",\"data_type\":\"uint16\",\"value\":\"0\"}]},{\"type\":"
        "\"Expression\","
        "\"id\":\"ReturnType\",\"declarations\":[{\"type\":\"Literal\",\"data_type\":\"To Be "
        "Determined\",\"value\":\"0\"}]}]}}],\"SourceType\":\"module\"}";
    EXPECT_STREQ(toJson(parse(std::move(source)), false).c_str(), result);
    EXPECT_FALSE(hasErrors());
}

TEST(AST, PARSE_MAIN_WITH_VAR_UINT32_WITHOUT_VALUE)
{
    const char *source = "func main() -> int32 { uint32 x ret 0}";
    const char *result =
        "{\"type\":\"Program\",\"body\":[{\"type\":\"FunctionDef\",\"id\":\"main\",\"return_type\":\"int32\","
        "\"parameters\":{\"type\":\"Expression\",\"id\":\"ParameterList\",\"declarations\":[]},\"body\":{\"type\":"
        "\"ExecutionBlock\",\"execution\":[{\"type\":\"Expression\",\"id\":\"VariableDefinition\",\"name\":\"x\","
        "\"declarations\":[{\"type\":\"Literal\",\"data_type\":\"uint32\",\"value\":\"0\"}]},{\"type\":"
        "\"Expression\","
        "\"id\":\"ReturnType\",\"declarations\":[{\"type\":\"Literal\",\"data_type\":\"To Be "
        "Determined\",\"value\":\"0\"}]}]}}],\"SourceType\":\"module\"}";
    EXPECT_STREQ(toJson(parse(std::move(source)), false).c_str(), result);
    EXPECT_FALSE(hasErrors());
}

TEST(AST, PARSE_MAIN_WITH_VAR_UINT64_WITHOUT_VALUE)
{
    const char *source = "func main() -> int32 { uint64 x ret 0}";
    const char *result =
        "{\"type\":\"Program\",\"body\":[{\"type\":\"FunctionDef\",\"id\":\"main\",\"return_type\":\"int32\","
        "\"parameters\":{\"type\":\"Expression\",\"id\":\"ParameterList\",\"declarations\":[]},\"body\":{\"type\":"
        "\"ExecutionBlock\",\"execution\":[{\"type\":\"Expression\",\"id\":\"VariableDefinition\",\"name\":\"x\","
        "\"declarations\":[{\"type\":\"Literal\",\"data_type\":\"uint64\",\"value\":\"0\"}]},{\"type\":"
        "\"Expression\","
        "\"id\":\"ReturnType\",\"declarations\":[{\"type\":\"Literal\",\"data_type\":\"To Be "
        "Determined\",\"value\":\"0\"}]}]}}],\"SourceType\":\"module\"}";
    EXPECT_STREQ(toJson(parse(std::move(source)), false).c_str(), result);
    EXPECT_FALSE(hasErrors());
}

TEST(AST, PARSE_MAIN_WITH_VAR_INT8_WITHOUT_VALUE)
{
    const char *source = "func main() -> int32 { int8 x ret 0}";
    const char *result =
        "{\"type\":\"Program\",\"body\":[{\"type\":\"FunctionDef\",\"id\":\"main\",\"return_type\":\"int32\","
        "\"parameters\":{\"type\":\"Expression\",\"id\":\"ParameterList\",\"declarations\":[]},\"body\":{\"type\":"
        "\"ExecutionBlock\",\"execution\":[{\"type\":\"Expression\",\"id\":\"VariableDefinition\",\"name\":\"x\","
        "\"declarations\":[{\"type\":\"Literal\",\"data_type\":\"int8\",\"value\":\"0\"}]},{\"type\":"
        "\"Expression\","
        "\"id\":\"ReturnType\",\"declarations\":[{\"type\":\"Literal\",\"data_type\":\"To Be "
        "Determined\",\"value\":\"0\"}]}]}}],\"SourceType\":\"module\"}";
    EXPECT_STREQ(toJson(parse(std::move(source)), false).c_str(), result);
    EXPECT_FALSE(hasErrors());
}

TEST(AST, PARSE_MAIN_WITH_VAR_INT16_WITHOUT_VALUE)
{
    const char *source = "func main() -> int32 { int16 x ret 0}";
    const char *result =
        "{\"type\":\"Program\",\"body\":[{\"type\":\"FunctionDef\",\"id\":\"main\",\"return_type\":\"int32\","
        "\"parameters\":{\"type\":\"Expression\",\"id\":\"ParameterList\",\"declarations\":[]},\"body\":{\"type\":"
        "\"ExecutionBlock\",\"execution\":[{\"type\":\"Expression\",\"id\":\"VariableDefinition\",\"name\":\"x\","
        "\"declarations\":[{\"type\":\"Literal\",\"data_type\":\"int16\",\"value\":\"0\"}]},{\"type\":"
        "\"Expression\","
        "\"id\":\"ReturnType\",\"declarations\":[{\"type\":\"Literal\",\"data_type\":\"To Be "
        "Determined\",\"value\":\"0\"}]}]}}],\"SourceType\":\"module\"}";
    EXPECT_STREQ(toJson(parse(std::move(source)), false).c_str(), result);
    EXPECT_FALSE(hasErrors());
}

TEST(AST, PARSE_MAIN_WITH_VAR_INT32_WITHOUT_VALUE)
{
    const char *source = "func main() -> int32 { int32 x ret 0}";
    const char *result =
        "{\"type\":\"Program\",\"body\":[{\"type\":\"FunctionDef\",\"id\":\"main\",\"return_type\":\"int32\","
        "\"parameters\":{\"type\":\"Expression\",\"id\":\"ParameterList\",\"declarations\":[]},\"body\":{\"type\":"
        "\"ExecutionBlock\",\"execution\":[{\"type\":\"Expression\",\"id\":\"VariableDefinition\",\"name\":\"x\","
        "\"declarations\":[{\"type\":\"Literal\",\"data_type\":\"int32\",\"value\":\"0\"}]},{\"type\":"
        "\"Expression\","
        "\"id\":\"ReturnType\",\"declarations\":[{\"type\":\"Literal\",\"data_type\":\"To Be "
        "Determined\",\"value\":\"0\"}]}]}}],\"SourceType\":\"module\"}";
    EXPECT_STREQ(toJson(parse(std::move(source)), false).c_str(), result);
    EXPECT_FALSE(hasErrors());
}

TEST(AST, PARSE_MAIN_WITH_VAR_INT64_WITHOUT_VALUE)
{
    const char *source = "func main() -> int32 { int64 x ret 0}";
    const char *result =
        "{\"type\":\"Program\",\"body\":[{\"type\":\"FunctionDef\",\"id\":\"main\",\"return_type\":\"int32\","
        "\"parameters\":{\"type\":\"Expression\",\"id\":\"ParameterList\",\"declarations\":[]},\"body\":{\"type\":"
        "\"ExecutionBlock\",\"execution\":[{\"type\":\"Expression\",\"id\":\"VariableDefinition\",\"name\":\"x\","
        "\"declarations\":[{\"type\":\"Literal\",\"data_type\":\"int64\",\"value\":\"0\"}]},{\"type\":"
        "\"Expression\","
        "\"id\":\"ReturnType\",\"declarations\":[{\"type\":\"Literal\",\"data_type\":\"To Be "
        "Determined\",\"value\":\"0\"}]}]}}],\"SourceType\":\"module\"}";
    EXPECT_STREQ(toJson(parse(std::move(source)), false).c_str(), result);
    EXPECT_FALSE(hasErrors());
}

TEST(AST, PARSE_MAIN_WITH_VAR_FLOAT32_WITHOUT_VALUE)
{
    const char *source = "func main() -> int32 { float32 x ret 0}";
    const char *result =
        "{\"type\":\"Program\",\"body\":[{\"type\":\"FunctionDef\",\"id\":\"main\",\"return_type\":\"int32\","
        "\"parameters\":{\"type\":\"Expression\",\"id\":\"ParameterList\",\"declarations\":[]},\"body\":{\"type\":"
        "\"ExecutionBlock\",\"execution\":[{\"type\":\"Expression\",\"id\":\"VariableDefinition\",\"name\":\"x\","
        "\"declarations\":[{\"type\":\"Literal\",\"data_type\":\"float32\",\"value\":\"0.0\"}]},{\"type\":"
        "\"Expression\",\"id\":\"ReturnType\",\"declarations\":[{\"type\":\"Literal\",\"data_type\":\"any "
        "uint\",\"value\":\"0\"}]}]}}],\"SourceType\":\"module\"}";
    EXPECT_STREQ(toJson(parse(std::move(source)), false).c_str(), result);
    EXPECT_FALSE(hasErrors());
}

TEST(AST, PARSE_MAIN_WITH_VAR_FLOAT64_WITHOUT_VALUE)
{
    const char *source = "func main() -> int32 { float64 x ret 0}";
    const char *result =
        "{\"type\":\"Program\",\"body\":[{\"type\":\"FunctionDef\",\"id\":\"main\",\"return_type\":\"int32\","
        "\"parameters\":{\"type\":\"Expression\",\"id\":\"ParameterList\",\"declarations\":[]},\"body\":{\"type\":"
        "\"ExecutionBlock\",\"execution\":[{\"type\":\"Expression\",\"id\":\"VariableDefinition\",\"name\":\"x\","
        "\"declarations\":[{\"type\":\"Literal\",\"data_type\":\"float64\",\"value\":\"0.0\"}]},{\"type\":"
        "\"Expression\",\"id\":\"ReturnType\",\"declarations\":[{\"type\":\"Literal\",\"data_type\":\"any "
        "uint\",\"value\":\"0\"}]}]}}],\"SourceType\":\"module\"}";
    EXPECT_STREQ(toJson(parse(std::move(source)), false).c_str(), result);
    EXPECT_FALSE(hasErrors());
}

TEST(AST, PARSE_MAIN_WITH_ANOTHER_FUNCTION)
{
    const char *source = "func get() -> uint64 { ret 0 } func main() -> int32 { ret 0}";
    const char *result =
        "{\"type\":\"Program\",\"body\":[{\"type\":\"FunctionDef\",\"id\":\"get\",\"return_type\":\"uint64\","
        "\"parameters\":{\"type\":\"Expression\",\"id\":\"ParameterList\",\"declarations\":[]},\"body\":{\"type\":"
        "\"ExecutionBlock\",\"execution\":[{\"type\":\"Expression\",\"id\":\"ReturnType\",\"declarations\":[{\"type\":"
        "\"Literal\",\"data_type\":\"any "
        "uint\",\"value\":\"0\"}]}]}},{\"type\":\"FunctionDef\",\"id\":\"main\",\"return_type\":\"int32\","
        "\"parameters\":{\"type\":\"Expression\",\"id\":\"ParameterList\",\"declarations\":[]},\"body\":{\"type\":"
        "\"ExecutionBlock\",\"execution\":[{\"type\":\"Expression\",\"id\":\"ReturnType\",\"declarations\":[{\"type\":"
        "\"Literal\",\"data_type\":\"any uint\",\"value\":\"0\"}]}]}}],\"SourceType\":\"module\"}";
    EXPECT_STREQ(toJson(parse(std::move(source)), false).c_str(), result);
    EXPECT_FALSE(hasErrors());
}

// flawed code tests
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

TEST(AST, PARSE_VAR_BAD_SYNTEX)
{
    const char *source_array[] = {"foat64 x = 20.0",        // 1
                                  "foat64 x",               // 2
                                  "float64 x - 20.0",       // 3
                                  "float64 x 20.0",         // 4
                                  "float64 x = "            // 5
                                  "float64 x = 0.0 mo 8.0", // 6
                                  "float64 x  0.0 mod 8.0", // 7
                                  "float64 x = 0.0 mod .0", // 8
                                  "in64 x = 20",            // 9
                                  "nt64 x",                 // 10
                                  "int64 x - 20",           // 11
                                  "int64 x 20",             // 12
                                  "int64 x = "              // 13
                                  "int64 x = 0.0 mo 8.0",   // 14
                                  "int64 x  0 mod 8",       // 15
                                  "int64 x = 0.0 mod "};    // 16
    uint8_t x = 1;
    for (const char *c : source_array)
    {
        printf("Test: %u -> ", x++);
        enableSoftErrors();
        (void)parse(std::move(c));
        EXPECT_TRUE(hasErrors());
        disableSoftErrors();
        puts("passed");
    }
}

TEST(AST, FUNCTION_BAD_SYNTEX)
{
    const char *source_array[] = {
        "main() -> int32 { ret 0}",      "func main) -> int32 { ret 0}", "func main() - int32 { ret 0}",
        "func main() -> int36 { ret 0}", "func main() -> int32 { rt 0}", "func main() -> int32 { ret . }",
        "func main() -> int32 { ret 0 ", "func main() -> int32  ret 0}", "func int32() -> int32 { ret 0}",
        "func main() > int32 { ret 0}",  "func main() - int32 { ret 0}", "fuc main() -> int32 { ret 0}"};
    uint8_t x = 1;
    for (const char *c : source_array)
    {
        printf("Test: %u -> ", x++);
        enableSoftErrors();
        (void)parse(std::move(c));
        EXPECT_TRUE(hasErrors());
        disableSoftErrors();
        puts("passed");
    }
}
#endif