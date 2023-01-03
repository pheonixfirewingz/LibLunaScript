#include "../Cmake.h"
#include "IO.h"
#include <liblunascript/Compiler.h>
#include <argparse/argparse.hpp>
#include <cstdio>

//int main(int argc, char **argv)
int main(int, char **)
{
    /* argparse::ArgumentParser program("LunaScript");

    program.add_argument("input").required().help("the sorce file name");
     program.add_argument("-o", "--output").required().help("specify the output file.");
    program.add_argument("-O", "--optimize").required().help("optimization level").default_value(0);

    try
    {
        program.parse_args(argc, argv);
    }
    catch (const std::runtime_error &err)
    {
        std::cerr << err.what() << std::endl;
        std::cerr << program;
        std::exit(1);
    }*/

    bool stop = false;
    setRoot(PROJECT_SOURCE_DIR);
    losResult res;
    Compiler compiler;
    char *read_str;
    data_size_t read_str_size = 0;
    //std::string path_1("$[asset_base]/" + program.get("input"));
    std::string path_1("$[asset_base]/test.lls");
    if(!path_1.ends_with(".lls"))
    {
        printf("ERROR: %s is not the correct file format",path_1.c_str());
        std::exit(1);
    }
    if ((res = fileRead(path_1.c_str(), path_1.size(), &read_str, &read_str_size)) != LOS_SUCCESS)
        return res;
    if (compileAST(&compiler,read_str,read_str_size,"test",4) != LOS_SUCCESS)
    {
        stop = true;
        while (hasErrorOnStack(compiler))
            puts(getErrorOffStack(compiler).c_str());
    }
    if (stop)
        return 0;
    auto ast = astToString(compiler);
    //std::string path_2("$[asset_base]/" + program.get("-o"));
    std::string path_2("$[asset_base]/test.ast");
    if ((res = fileWrite(path_2.c_str(), path_2.size(), ast.c_str(), ast.size())) != LOS_SUCCESS)
        if ((res = fileWrite(path_2.c_str(), path_2.size(), ast.c_str(), ast.size(),false)) != LOS_SUCCESS)
            return res;
    freeCompiler(compiler);
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