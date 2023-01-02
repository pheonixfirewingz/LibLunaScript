#include "../Cmake.h"
#if COMPILER_TESTING == 0
#include "IO.h"
#include "back/IRCodeGen.h"
#include "front/Parser.h"
#include <argparse/argparse.hpp>
int main(int, char **)
{
    bool stop = false;
    setRoot("home/digitech/Desktop/LibLunaScript");
    losResult res;
    char *read_str;
    data_size_t read_str_size = 0;
    if ((res = fileRead("$[asset_base]/test.lls", 23, &read_str, &read_str_size)) != LOS_SUCCESS)
        return res;
    LunaScript::compiler::front::Parser parser(std::move(std::string((char *)read_str, 0, read_str_size)), "test");
    //const ASTRoot const*root_ast = parser.getAST();
    if (parser.hasErrors())
    {
        stop = true;
        while (parser.hasErrors())
            puts(parser.popErrorOffStack().c_str());
    }
    if (stop)
        return 0;
    auto ast = parser.asString(true);
    if ((res = fileWrite("$[asset_base]/test.lls.ast", 27, ast.c_str(), ast.size())) != LOS_SUCCESS)
        return res;
   /* auto ir = CodeGen::LLVMCodeGen(root_ast).getIR();
    if ((res = fileWrite("$[asset_base]/test.ll", 23, ir.c_str(), ir.size())) != LOS_SUCCESS)
        return res;
    ir->setTargetTriple(target_triple);
    std::string Error;
    auto Target = TargetRegistry::lookupTarget(target_triple, Error);
    if (!Target)
    {
        errs() << Error;
        return 1;
    }

    auto CPU = "generic";
    auto Features = "";

    TargetOptions opt;
    auto RM = Optional<Reloc::Model>();
    auto TheTargetMachine = Target->createTargetMachine(target_triple, CPU, Features, opt, RM);

    ir->setDataLayout(TheTargetMachine->createDataLayout());
    legacy::PassManager pass;
    std::string human_readable_2;
    raw_string_ostream OS(human_readable_2);
    if (TheTargetMachine->addPassesToEmitFile(pass, (raw_pwrite_stream &)OS, nullptr, CGFT_AssemblyFile))
    {
        errs() << "TargetMachine can't emit a file of this type";
        return 1;
    }

    pass.run(*ir);
    OS.flush();

    if ((res = fileWrite("$[asset_base]/test.asm", 24, human_readable_2.c_str(), human_readable_2.size())) !=
        LOS_SUCCESS)
        return res;*/
    return 0;
}
#if FUZZING == 1
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
    enableSoftErrors();
    std::string x((char *)Data, 0, Size);
    const ASTRoot *v = parse(std::move(x));
    delete v;
    int ret = hasErrors() ? 0 : -1;
    if (ret == -1)
        puts("did error correctly");
    disableSoftErrors();
    return ret; // Values other than 0 and -1 are reserved for future use.
}
#endif
#endif