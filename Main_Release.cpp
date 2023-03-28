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

int main(int, char **)
{
    libOSInit();
    losResult res;
    setRoot(PROJECT_SOURCE_DIR);
    char *src;
    data_size_t src_size = 0;
    if ((res = fileRead(createP("", "test", ".lls"), &src, &src_size)) != LOS_SUCCESS)
        return res;
    puts(std::string("\x1B[32mCompiler Output:\x1B[33m").c_str());
    LunaScriptCompiler compile(std::string(src, 0, src_size), true);
    delete src;
    if (compile.didScriptCompile() != LOS_SUCCESS)
    {
#    if ON_WINDOWS
        system("pause");
#    endif
        return compile.didScriptCompile();
    }
    puts(std::string("\x1B[32mAst:\x1B[33m\n").c_str());
    const std::string ast = compile.getJsonAST();
    puts(ast.c_str());
    libOSCleanUp();
#    if ON_WINDOWS
    system("pause");
#    endif
    return 0;
}
#endif