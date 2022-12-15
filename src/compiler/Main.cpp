#include "../Cmake.h"
#include "Lexer.h"
#include "Parser.h"
#ifndef TESTS_MODE

int main(int, char **)
{
    const char *source = "";
    enableSoftErrors();
    auto ast = toJson(parse(std::move(lexIt(std::move(source)))),true);
    while (hasErrors())
        printf("COMPILER ERROR: %s\n", popSoftErrorOffStack());
    disableSoftErrors();
    puts(ast.c_str());
    return 0;
}
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
    EXPECT_STREQ(toJson(parse(std::move(lexIt(std::move(source)))), false).c_str(), result);
}

TEST(AST, PARSE_VAR_UINT16_NO_VALUE)
{
    const char *source = "uint16 x";
    const char *result = "{\"type\":\"Program\",\"body\":[{\"type\":\"Expression\",\"id\":\"VariableDefinition\","
                         "\"name\":\"x\",\"declarations\":[{\"type\":\"Literal\",\"data_type\":\"uint16\",\"value\":"
                         "\"0\"}]}],\"SourceType\":\"module\"}";
    EXPECT_STREQ(toJson(parse(std::move(lexIt(std::move(source)))), false).c_str(), result);
}

TEST(AST, PARSE_VAR_UINT32_NO_VALUE)
{
    const char *source = "uint32 x";
    const char *result = "{\"type\":\"Program\",\"body\":[{\"type\":\"Expression\",\"id\":\"VariableDefinition\","
                         "\"name\":\"x\",\"declarations\":[{\"type\":\"Literal\",\"data_type\":\"uint32\",\"value\":"
                         "\"0\"}]}],\"SourceType\":\"module\"}";
    EXPECT_STREQ(toJson(parse(std::move(lexIt(std::move(source)))), false).c_str(), result);
}

TEST(AST, PARSE_VAR_UINT64_NO_VALUE)
{
    const char *source = "uint64 x";
    const char *result = "{\"type\":\"Program\",\"body\":[{\"type\":\"Expression\",\"id\":\"VariableDefinition\","
                         "\"name\":\"x\",\"declarations\":[{\"type\":\"Literal\",\"data_type\":\"uint64\",\"value\":"
                         "\"0\"}]}],\"SourceType\":\"module\"}";
    EXPECT_STREQ(toJson(parse(std::move(lexIt(std::move(source)))), false).c_str(), result);
}

TEST(AST, PARSE_VAR_INT8_NO_VALUE)
{
    const char *source = "int8 x";
    const char *result =
        "{\"type\":\"Program\",\"body\":[{\"type\":\"Expression\",\"id\":\"VariableDefinition\",\"name\":\"x\","
        "\"declarations\":[{\"type\":\"Literal\",\"data_type\":\"int8\",\"value\":\"0\"}]}],\"SourceType\":\"module\"}";
    EXPECT_STREQ(toJson(parse(std::move(lexIt(std::move(source)))), false).c_str(), result);
}

TEST(AST, PARSE_VAR_INT16_NO_VALUE)
{
    const char *source = "int16 x";
    const char *result = "{\"type\":\"Program\",\"body\":[{\"type\":\"Expression\",\"id\":\"VariableDefinition\","
                         "\"name\":\"x\",\"declarations\":[{\"type\":\"Literal\",\"data_type\":\"int16\",\"value\":"
                         "\"0\"}]}],\"SourceType\":\"module\"}";
    EXPECT_STREQ(toJson(parse(std::move(lexIt(std::move(source)))), false).c_str(), result);
}

TEST(AST, PARSE_VAR_INT32_NO_VALUE)
{
    const char *source = "int32 x";
    const char *result = "{\"type\":\"Program\",\"body\":[{\"type\":\"Expression\",\"id\":\"VariableDefinition\","
                         "\"name\":\"x\",\"declarations\":[{\"type\":\"Literal\",\"data_type\":\"int32\",\"value\":"
                         "\"0\"}]}],\"SourceType\":\"module\"}";
    EXPECT_STREQ(toJson(parse(std::move(lexIt(std::move(source)))), false).c_str(), result);
}

TEST(AST, PARSE_VAR_INT64_NO_VALUE)
{
    const char *source = "int64 x";
    const char *result = "{\"type\":\"Program\",\"body\":[{\"type\":\"Expression\",\"id\":\"VariableDefinition\","
                         "\"name\":\"x\",\"declarations\":[{\"type\":\"Literal\",\"data_type\":\"int64\",\"value\":"
                         "\"0\"}]}],\"SourceType\":\"module\"}";
    EXPECT_STREQ(toJson(parse(std::move(lexIt(std::move(source)))), false).c_str(), result);
}

TEST(AST, PARSE_VAR_FLOAT32_NO_VALUE)
{
    const char *source = "float32 x";
    const char *result = "{\"type\":\"Program\",\"body\":[{\"type\":\"Expression\",\"id\":\"VariableDefinition\","
                         "\"name\":\"x\",\"declarations\":[{\"type\":\"Literal\",\"data_type\":\"float32\",\"value\":"
                         "\"0.0\"}]}],\"SourceType\":\"module\"}";
    EXPECT_STREQ(toJson(parse(std::move(lexIt(std::move(source)))), false).c_str(), result);
}

TEST(AST, PARSE_VAR_FLOAT64_NO_VALUE)
{
    const char *source = "float64 x";
    const char *result = "{\"type\":\"Program\",\"body\":[{\"type\":\"Expression\",\"id\":\"VariableDefinition\","
                         "\"name\":\"x\",\"declarations\":[{\"type\":\"Literal\",\"data_type\":\"float64\",\"value\":"
                         "\"0.0\"}]}],\"SourceType\":\"module\"}";
    EXPECT_STREQ(toJson(parse(std::move(lexIt(std::move(source)))), false).c_str(), result);
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
TEST(AST, PARSE_VAR_UINT8_WITH_VALUE)
{
    const char *source = "uint8 x = 20";
    const char *result = "{\"type\":\"Program\",\"body\":[{\"type\":\"Expression\",\"id\":\"VariableDefinition\","
                         "\"name\":\"x\",\"declarations\":[{\"type\":\"Literal\",\"data_type\":\"uint8\",\"value\":"
                         "\"20\"}]}],\"SourceType\":\"module\"}";
    EXPECT_STREQ(toJson(parse(std::move(lexIt(std::move(source)))), false).c_str(), result);
}

TEST(AST, PARSE_VAR_UINT16_WITH_VALUE)
{
    const char *source = "uint16 x = 20";
    const char *result = "{\"type\":\"Program\",\"body\":[{\"type\":\"Expression\",\"id\":\"VariableDefinition\","
                         "\"name\":\"x\",\"declarations\":[{\"type\":\"Literal\",\"data_type\":\"uint16\",\"value\":"
                         "\"20\"}]}],\"SourceType\":\"module\"}";
    EXPECT_STREQ(toJson(parse(std::move(lexIt(std::move(source)))), false).c_str(), result);
}

TEST(AST, PARSE_VAR_UINT32_WITH_VALUE)
{
    const char *source = "uint32 x = 20";
    const char *result = "{\"type\":\"Program\",\"body\":[{\"type\":\"Expression\",\"id\":\"VariableDefinition\","
                         "\"name\":\"x\",\"declarations\":[{\"type\":\"Literal\",\"data_type\":\"uint32\",\"value\":"
                         "\"20\"}]}],\"SourceType\":\"module\"}";
    EXPECT_STREQ(toJson(parse(std::move(lexIt(std::move(source)))), false).c_str(), result);
}

TEST(AST, PARSE_VAR_UINT64_WITH_VALUE)
{
    const char *source = "uint64 x = 20";
    const char *result = "{\"type\":\"Program\",\"body\":[{\"type\":\"Expression\",\"id\":\"VariableDefinition\","
                         "\"name\":\"x\",\"declarations\":[{\"type\":\"Literal\",\"data_type\":\"uint64\",\"value\":"
                         "\"20\"}]}],\"SourceType\":\"module\"}";
    EXPECT_STREQ(toJson(parse(std::move(lexIt(std::move(source)))), false).c_str(), result);
}

TEST(AST, PARSE_VAR_INT8_WITH_VALUE)
{
    const char *source = "int8 x = 20";
    const char *result = "{\"type\":\"Program\",\"body\":[{\"type\":\"Expression\",\"id\":\"VariableDefinition\","
                         "\"name\":\"x\",\"declarations\":[{\"type\":\"Literal\",\"data_type\":\"int8\",\"value\":"
                         "\"20\"}]}],\"SourceType\":\"module\"}";
    EXPECT_STREQ(toJson(parse(std::move(lexIt(std::move(source)))), false).c_str(), result);
}

TEST(AST, PARSE_VAR_INT16_WITH_VALUE)
{
    const char *source = "int16 x = 20";
    const char *result = "{\"type\":\"Program\",\"body\":[{\"type\":\"Expression\",\"id\":\"VariableDefinition\","
                         "\"name\":\"x\",\"declarations\":[{\"type\":\"Literal\",\"data_type\":\"int16\",\"value\":"
                         "\"20\"}]}],\"SourceType\":\"module\"}";
    EXPECT_STREQ(toJson(parse(std::move(lexIt(std::move(source)))), false).c_str(), result);
}

TEST(AST, PARSE_VAR_INT32_WITH_VALUE)
{
    const char *source = "int32 x = 20";
    const char *result = "{\"type\":\"Program\",\"body\":[{\"type\":\"Expression\",\"id\":\"VariableDefinition\","
                         "\"name\":\"x\",\"declarations\":[{\"type\":\"Literal\",\"data_type\":\"int32\",\"value\":"
                         "\"20\"}]}],\"SourceType\":\"module\"}";
    EXPECT_STREQ(toJson(parse(std::move(lexIt(std::move(source)))), false).c_str(), result);
}

TEST(AST, PARSE_VAR_INT64_WITH_VALUE)
{
    const char *source = "int64 x = 20";
    const char *result = "{\"type\":\"Program\",\"body\":[{\"type\":\"Expression\",\"id\":\"VariableDefinition\","
                         "\"name\":\"x\",\"declarations\":[{\"type\":\"Literal\",\"data_type\":\"int64\",\"value\":"
                         "\"20\"}]}],\"SourceType\":\"module\"}";
    EXPECT_STREQ(toJson(parse(std::move(lexIt(std::move(source)))), false).c_str(), result);
}

TEST(AST, PARSE_VAR_FLOAT32_WITH_VALUE)
{
    const char *source = "float32 x = 20.0";
    const char *result = "{\"type\":\"Program\",\"body\":[{\"type\":\"Expression\",\"id\":\"VariableDefinition\","
                         "\"name\":\"x\",\"declarations\":[{\"type\":\"Literal\",\"data_type\":\"float32\",\"value\":"
                         "\"20.0\"}]}],\"SourceType\":\"module\"}";
    EXPECT_STREQ(toJson(parse(std::move(lexIt(std::move(source)))), false).c_str(), result);
}

TEST(AST, PARSE_VAR_FLOAT64_WITH_VALUE)
{
    const char *source = "float64 x = 20.0";
    const char *result = "{\"type\":\"Program\",\"body\":[{\"type\":\"Expression\",\"id\":\"VariableDefinition\","
                         "\"name\":\"x\",\"declarations\":[{\"type\":\"Literal\",\"data_type\":\"float64\",\"value\":"
                         "\"20.0\"}]}],\"SourceType\":\"module\"}";
    EXPECT_STREQ(toJson(parse(std::move(lexIt(std::move(source)))), false).c_str(), result);
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
    EXPECT_STREQ(toJson(parse(std::move(lexIt(std::move(source)))), false).c_str(), result);
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
    EXPECT_STREQ(toJson(parse(std::move(lexIt(std::move(source)))), false).c_str(), result);
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
    EXPECT_STREQ(toJson(parse(std::move(lexIt(std::move(source)))), false).c_str(), result);
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
    EXPECT_STREQ(toJson(parse(std::move(lexIt(std::move(source)))), false).c_str(), result);
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
    EXPECT_STREQ(toJson(parse(std::move(lexIt(std::move(source)))), false).c_str(), result);
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
    EXPECT_STREQ(toJson(parse(std::move(lexIt(std::move(source)))), false).c_str(), result);
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
    EXPECT_STREQ(toJson(parse(std::move(lexIt(std::move(source)))), false).c_str(), result);
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
    EXPECT_STREQ(toJson(parse(std::move(lexIt(std::move(source)))), false).c_str(), result);
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
    EXPECT_STREQ(toJson(parse(std::move(lexIt(std::move(source)))), false).c_str(), result);
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
    EXPECT_STREQ(toJson(parse(std::move(lexIt(std::move(source)))), false).c_str(), result);
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
    EXPECT_STREQ(toJson(parse(std::move(lexIt(std::move(source)))), false).c_str(), result);
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
    EXPECT_STREQ(toJson(parse(std::move(lexIt(std::move(source)))), false).c_str(), result);
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
    EXPECT_STREQ(toJson(parse(std::move(lexIt(std::move(source)))), false).c_str(), result);
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
    EXPECT_STREQ(toJson(parse(std::move(lexIt(std::move(source)))), false).c_str(), result);
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
    EXPECT_STREQ(toJson(parse(std::move(lexIt(std::move(source)))), false).c_str(), result);
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
    EXPECT_STREQ(toJson(parse(std::move(lexIt(std::move(source)))), false).c_str(), result);
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
    EXPECT_STREQ(toJson(parse(std::move(lexIt(std::move(source)))), false).c_str(), result);
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
    EXPECT_STREQ(toJson(parse(std::move(lexIt(std::move(source)))), false).c_str(), result);
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
    EXPECT_STREQ(toJson(parse(std::move(lexIt(std::move(source)))), false).c_str(), result);
}

TEST(AST, PARSE_MAIN_WITH_VAR_FLOAT32_WITHOUT_VALUE)
{
    const char *source = "func main() -> int32 { float32 x ret 0}";
    const char *result =
        "{\"type\":\"Program\",\"body\":[{\"type\":\"FunctionDef\",\"id\":\"main\",\"return_type\":\"int32\","
        "\"parameters\":{\"type\":\"Expression\",\"id\":\"ParameterList\",\"declarations\":[]},\"body\":{\"type\":"
        "\"ExecutionBlock\",\"execution\":[{\"type\":\"Expression\",\"id\":\"VariableDefinition\",\"name\":\"x\","
        "\"declarations\":[{\"type\":\"Literal\",\"data_type\":\"float32\",\"value\":\"0.0\"}]},{\"type\":"
        "\"Expression\",\"id\":\"ReturnType\",\"declarations\":[{\"type\":\"Literal\",\"data_type\":\"To Be "
        "Determined\",\"value\":\"0\"}]}]}}],\"SourceType\":\"module\"}";
    EXPECT_STREQ(toJson(parse(std::move(lexIt(std::move(source)))), false).c_str(), result);
}

TEST(AST, PARSE_MAIN_WITH_VAR_FLOAT64_WITHOUT_VALUE)
{
    const char *source = "func main() -> int32 { float64 x ret 0}";
    const char *result =
        "{\"type\":\"Program\",\"body\":[{\"type\":\"FunctionDef\",\"id\":\"main\",\"return_type\":\"int32\","
        "\"parameters\":{\"type\":\"Expression\",\"id\":\"ParameterList\",\"declarations\":[]},\"body\":{\"type\":"
        "\"ExecutionBlock\",\"execution\":[{\"type\":\"Expression\",\"id\":\"VariableDefinition\",\"name\":\"x\","
        "\"declarations\":[{\"type\":\"Literal\",\"data_type\":\"float64\",\"value\":\"0.0\"}]},{\"type\":"
        "\"Expression\",\"id\":\"ReturnType\",\"declarations\":[{\"type\":\"Literal\",\"data_type\":\"To Be "
        "Determined\",\"value\":\"0\"}]}]}}],\"SourceType\":\"module\"}";
    EXPECT_STREQ(toJson(parse(std::move(lexIt(std::move(source)))), false).c_str(), result);
}

TEST(AST, PARSE_MAIN_WITH_ANOTHER_FUNCTION)
{
    const char *source = "func get() -> uint64 { ret 0 } func main() -> int32 { ret 0}";
    const char *result =
        "{\"type\":\"Program\",\"body\":[{\"type\":\"FunctionDef\",\"id\":\"get\",\"return_type\":\"uint64\","
        "\"parameters\":{\"type\":\"Expression\",\"id\":\"ParameterList\",\"declarations\":[]},\"body\":{\"type\":"
        "\"ExecutionBlock\",\"execution\":[{\"type\":\"Expression\",\"id\":\"ReturnType\",\"declarations\":[{"
        "\"type\":"
        "\"Literal\",\"data_type\":\"To Be "
        "Determined\",\"value\":\"0\"}]}]}},{\"type\":\"FunctionDef\",\"id\":\"main\",\"return_type\":\"int32\","
        "\"parameters\":{\"type\":\"Expression\",\"id\":\"ParameterList\",\"declarations\":[]},\"body\":{\"type\":"
        "\"ExecutionBlock\",\"execution\":[{\"type\":\"Expression\",\"id\":\"ReturnType\",\"declarations\":[{"
        "\"type\":"
        "\"Literal\",\"data_type\":\"To Be Determined\",\"value\":\"0\"}]}]}}],\"SourceType\":\"module\"}";
    EXPECT_STREQ(toJson(parse(std::move(lexIt(std::move(source)))), false).c_str(), result);
}

// flawed code tests
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

TEST(AST, PARSE_VAR_BAD_SYNTEX)
{
    const char *source_array[] = {"foat64 x = 20.0", "foat64 x", "float64 x - 20.0", "float64 x 20.0", "float64 x = "};
    for (const char *c : source_array)
    {
        enableSoftErrors();
        (void)parse(std::move(lexIt(std::move(c))));
        EXPECT_TRUE(hasErrors());
        disableSoftErrors();
    }
}

TEST(AST, FUNCTION_BAD_SYNTEX)
{
    const char *source_array[] = {
        "main() -> int32 { ret 0}",      "func main) -> int32 { ret 0}", "func main() - int32 { ret 0}",
        "func main() -> int36 { ret 0}", "func main() -> int32 { rt 0}", "func main() -> int32 { ret . }",
        "func main() -> int32 { ret 0 ", "func main() -> int32  ret 0}", "func int32() -> int32 { ret 0}",
        "func main() > int32 { ret 0}",  "func main() - int32 { ret 0}", "fuc main() -> int32 { ret 0}"};
    for (const char *c : source_array)
    {
        enableSoftErrors();
        (void)parse(std::move(lexIt(std::move(c))));
        EXPECT_TRUE(hasErrors());
        disableSoftErrors();
    }
}
#endif