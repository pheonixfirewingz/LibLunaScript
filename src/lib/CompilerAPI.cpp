#include <liblunascript/Compiler.h>
#include "front/Parser.h"

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

std::string getErrorOffStack(Compiler compiler)
{
    return compiler->parser->popErrorOffStack();
}

std::string astToString(Compiler compiler)
{
    return compiler->parser->asString(true);
}

void freeCompiler(Compiler compiler)
{
    delete compiler->parser;
    delete compiler;
}