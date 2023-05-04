#include <liblunascript/Compiler.h>

losResult compile(Compiler *, const char *, const size_t, const uint8_t)
{
    return LOS_ERROR_COULD_NOT_INIT;
}

uint8_t hasErrorOnStack(Compiler)
{
    return 0;
}

void getErrorOffStack(Compiler, char **, size_t *)
{
}

void astToString(Compiler, char **, size_t *)
{
}

void freeCompiler(Compiler)
{
}

void freeString(char *)
{
}

// testing new parser
#include "AST.h"
#include "parser/Lexer.h"
#include "parser/Parser.h"
#include "parser/Translator.h"
#include <algorithm>
#include <iostream>
#include <rapidjson/prettywriter.h>
#include <rapidjson/stringbuffer.h>
#include <string>

static inline std::wstring prep(const wchar_t *src, const size_t src_size)
{
    std::wstring str(src, 0, src_size);
    if (str.contains(L'\r'))
        str.erase(std::remove(str.begin(), str.end(), L'\r'), str.end());
    if (str.contains(L';'))
        str.erase(std::remove(str.begin(), str.end(), L';'), str.end());
    if (str.starts_with(L"lang"))
        return LunaScript::parser::lexer::Translator{}(str);
    else
        return str;
}

namespace LunaScript::compiler::front
{
using namespace LunaScript::compiler::ast;
using namespace rapidjson;
extern void writeBranch(PrettyWriter<GenericStringBuffer<UTF16<>>, UTF16<>> *, const ASTNode *);
} // namespace LunaScript::compiler::front

void newCompile(const wchar_t *src, const size_t src_size, wchar_t **data_out, size_t *data_out_size)
{
    const std::wstring source = prep(src, src_size);
    std::vector<LunaScript::parser::lexer::TypedStringView> tokens = LunaScript::parser::lexer::Lexer{}(source);
    rapidjson::GenericStringBuffer<rapidjson::UTF16<>> s;
    rapidjson::PrettyWriter<rapidjson::GenericStringBuffer<rapidjson::UTF16<>>, rapidjson::UTF16<>> writer(s);
    LunaScript::compiler::front::writeBranch(&writer, LunaScript::parser::Parser{}(tokens));
    const wchar_t *data = s.GetString();
    *data_out_size = std::char_traits<wchar_t>::length(data);
    *data_out = new wchar_t[(*data_out_size + 1) * sizeof(wchar_t)];
    memset(*data_out, 0, *data_out_size * sizeof(wchar_t));
    memcpy(*data_out, data, *data_out_size * sizeof(wchar_t));
}