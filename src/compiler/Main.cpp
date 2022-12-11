#include "../Cmake.h"
#include <string>
#include <vector>

extern std::vector<std::string> tokenizeString(const std::string && str);

#if CMAKE_LIB_MODE == 0
int main(int, char *[])
{
    std::string str{std::string("                                                                                \n") +=
                    std::string("func main() -> int32                                                            \n") +=
                    std::string("{                                                                               \n") +=
                    std::string("    int32 a                                                                     \n") +=
                    std::string("    int32 b                                                                     \n") +=
                    std::string("    int32 output                                                                \n") +=
                    std::string("    int8 input                                                                  \n") +=
                    std::string("                                                                                \n") +=
                    std::string("    console.println(\"Enter Value 1 and Value 2 : \")                           \n") +=
                    std::string("                                                                                \n") +=
                    std::string("    a = console.read()                                                          \n") +=
                    std::string("    b = console.read()                                                          \n") +=
                    std::string("                                                                                \n") +=
                    std::string("    console.println(\"Enter The Arithmetic Operation You Want (+, -, /, x) : \")\n") +=
                    std::string("                                                                                \n") +=
                    std::string("    input = console.read()                                                      \n") +=
                    std::string("                                                                                \n") +=
                    std::string("    if input equals '+': output = a + b                                         \n") +=
                    std::string("    elif input equals '-': output = a - b                                       \n") +=
                    std::string("    elif input equals '/': output = a / b                                       \n") +=
                    std::string("    elif input equals '-': output = a - b                                       \n") +=
                    std::string("    elif input equals 'x': output = a * b                                       \n") +=
                    std::string("    else: console.println(\"Invalid Input Operation\")                          \n") +=
                    std::string("                                                                                \n") +=
                    std::string("    console.println(output)                                                     \n") +=
                    std::string("    ret 0                                                                       \n") +=
                    std::string("}")};

    for (auto &str : tokenizeString(std::move(str)))
        printf("|%s|",str.c_str());
    return 0;
}
#endif