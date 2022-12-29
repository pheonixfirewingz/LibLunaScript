#include "../AST.h"
#include <llvm-15/llvm/IR/Module.h>
#include <string>
#include <stdint.h>

using namespace llvm;

Module* generateIRCode(const ASTRoot* root) noexcept;
std::string makeHumanReadable(const Module* module) noexcept;