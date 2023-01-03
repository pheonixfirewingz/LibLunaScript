#include "front/Parser.h"
#include <cstring>
#include <liblunascript/Compiler.h>

struct Compiler_T
{
    LunaScript::compiler::front::Parser *parser;
};

losResult compileAST(Compiler *compiler, const char *src, const data_size_t, const char *filename, const data_size_t)
{
    if ((*compiler))
    {
        *compiler = new Compiler_T();
        (*compiler)->parser = new LunaScript::compiler::front::Parser(src, filename);
        if ((*compiler)->parser->hasErrors())
            return LOS_ERROR_MALFORMED_DATA;
    }
    return LOS_SUCCESS;
}

uint8_t hasErrorOnStack(Compiler compiler)
{
    return compiler->parser->hasErrors();
}

void getErrorOffStack(Compiler compiler, char **str, data_size_t *str_size)
{
    std::string err = compiler->parser->popErrorOffStack();
    (*str) = new char[(*str_size = err.size())];
    strcpy(*str, err.c_str());
}

void astToString(Compiler compiler, char **str, data_size_t *str_size)
{
    std::string err = compiler->parser->asString(true);
    (*str) = new char[(*str_size = err.size())];
    strcpy(*str, err.c_str());
}

void freeCompiler(Compiler compiler)
{
    delete compiler->parser;
    delete compiler;
}