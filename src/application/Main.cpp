#include "Cmake.h"
#include "IO.hpp"
#include <algorithm>
#include <argparse/argparse.hpp>
#include <cstdio>
#include <liblunascript/Compiler.h>
#include <string>

#if FUZZING == 1
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
#else
static const char *const words[] = {"Const","Add","Sub","Div","Mul","Jmp","Cmp","Ncmp","Push","Pop","Store","Load","Call","Ret","Mov","Skip"};

void replace_all(std::string &inout, std::string_view what, std::string_view with)
{
    std::size_t count{};
    for (std::string::size_type pos{}; inout.npos != (pos = inout.find(what.data(), pos, what.length()));
         pos += with.length(), ++count)
        inout.replace(pos, what.length(), with.data(), with.length());
}

char tolower(char in) {
    if (in <= 'Z' && in >= 'A')
        return in - ('Z' - 'z');
    return in;
}

std::string colourText(std::string text)
{
    for(uint16_t i = 0; i < sizeof(words) / sizeof(const char*);i++)
    {
        std::string word(words[i]);
        std::string e("\x1B[94m");
        if(word.find("Const") == std::string::npos)
            e += "    ";
        e += word;
        e += "\x1B[00m";
        std::string t;
        t += tolower(word[0]);
        t += std::string(word.c_str(),1,word.size());
        replace_all(text,t,e);
        replace_all(text,word,t);
    }  
    return text;
}

std::string indentText(std::string text)
{
    for(uint16_t i = 0; i < sizeof(words) / sizeof(const char*);i++)
    {
        std::string word(words[i]);
        std::string e("");
        if(word.find("Const") == std::string::npos)
            e += "    ";
        e += word;
        std::string t;
        t += tolower(word[0]);
        t += std::string(word.c_str(),1,word.size());
        replace_all(text,t,e);
        replace_all(text,word,t);
    }  
    return text;
}

int main(int, char **)
{
    bool gen_debug = true;
    libOSInit();
    losResult res;
    setRoot(PROJECT_SOURCE_DIR);
    char *src;
    data_size_t src_size = 0;
    const char *name = "test";
    if ((res = fileRead(createP("", name, ".lls"), &src, &src_size)) != LOS_SUCCESS)
        return res;
    LunaScriptCompiler compile(std::string(src, 0, src_size), name);
    if (compile.didScriptCompile() != LOS_SUCCESS)
    {
        puts(compile.getErrors().c_str());
        return compile.didScriptCompile();
    }
    puts(std::string("\x1B[94mLunaScriptCodeAST:\x1B[33m\n").c_str());
    puts(compile.getJsonAST().c_str());

    if (!gen_debug)
    {
        auto bytecode = indentText(compile.getByteCode());

        if ((res = fileWrite<char>(createP("", "test", ".lsasm"), bytecode.c_str(), bytecode.size())) != LOS_SUCCESS)
            return res;
    }
    else
    {
        puts(std::string("\n\n\x1B[94mLunaScriptCodeGen:\x1B[00m\n").c_str());
        puts(colourText(compile.getByteCode()).c_str());
    }
    libOSCleanUp();
    return 0;
}
#endif