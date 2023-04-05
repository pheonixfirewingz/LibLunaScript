#include "front/Parser.h"
#include <algorithm>
#include <cstring>
#include <liblunascript/Compiler.h>
using namespace LunaScript;

struct Compiler_T
{
    LunaScript::compiler::front::Parser *parser;
};

losResult compile(Compiler *compiler, const char *src, const size_t src_size, const uint8_t debug)
{
    if (*compiler || *compiler == nullptr)
    {
        *compiler = new Compiler_T();
        (*compiler)->parser = new LunaScript::compiler::front::Parser(std::string(src, 0, src_size), debug);
        if ((*compiler)->parser->hasErrors())
            return LOS_ERROR_MALFORMED_DATA;
        return LOS_SUCCESS;
    }
    return LOS_ERROR_COULD_NOT_INIT;
}

uint8_t hasErrorOnStack(Compiler compiler)
{
    return compiler->parser->hasErrors();
}

void getErrorOffStack(Compiler compiler, char **str, size_t *str_size)
{
    std::string err = compiler->parser->popErrorOffStack();
    (*str) = new char[(*str_size = err.size())];
    memmove(*str, err.data(), *str_size);
}

void astToString(Compiler compiler, char **str, size_t *str_size)
{
    std::string err = compiler->parser->asString(true);
    (*str) = new char[(*str_size = err.size())];
    memmove(*str, err.data(), *str_size);
}

void freeCompiler(Compiler compiler)
{
    delete compiler->parser;
    delete compiler;
}

void freeString(char *src)
{
    free(src);
}

// testing new parser
#include "parser/Lexer.h"

static inline std::wstring prep(const wchar_t *src, const size_t src_size)
{
    std::wstring str(src, 0, src_size);
    if (str.contains(L'\r'))
        str.erase(std::remove(str.begin(), str.end(), L'\r'), str.end());
    if (str.contains(L';'))
        str.erase(std::remove(str.begin(), str.end(), L';'), str.end());
    return str;
}

static constexpr inline bool isUnicode(const wchar_t &c) noexcept
{
    return (c & 128) != 0;
}


void testFeature(const wchar_t *src, const size_t src_size)
{
    const std::wstring source = prep(src, src_size);
    std::vector<parser::lexer::TypedStringView> tokens = LunaScript::parser::lexer::Lexer{}(source);
    for (auto &t : tokens)
    {
        t.print(source);
    }
}