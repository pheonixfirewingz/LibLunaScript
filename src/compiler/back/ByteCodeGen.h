#include "LunaBytecode.h"
#include "../AST.h"
#include <string>
#include <stdint.h>

const std::vector<uint64_t> generateByteCode(const ASTRoot* root) noexcept;
std::string makeHumanReadable(const std::vector<uint64_t> ops) noexcept;