#include "front/Parser.h"
#include <cstring>
#include <liblunascript/Compiler.h>
using namespace LunaScript;

struct Compiler_T
{
    LunaScript::compiler::front::Parser *parser;
};

losResult compile(Compiler *compiler, const char *src, const data_size_t src_size, const uint8_t debug)
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

void getErrorOffStack(Compiler compiler, char **str, data_size_t *str_size)
{
    std::string err = compiler->parser->popErrorOffStack();
    (*str) = new char[(*str_size = err.size())];
    memmove(*str, err.data(), *str_size);
}

void astToString(Compiler compiler, char **str, data_size_t *str_size)
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

void freeString(char* src)
{
    free(src);
}

//testing new parser
#include "parser/Lexer.h"


void testFeature(const char8_t *src, const data_size_t src_size)
{
    std::unicodestring source(src,0, src_size);
    std::vector<parser::lexer::LexicalTypedSpan> tokens = parser::lexer::tokenize(source);
    for (auto& t : tokens)
    {
        t.print(source);
    }
}