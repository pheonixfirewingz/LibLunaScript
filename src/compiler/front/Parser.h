#pragma once
#include "../AST.h"

const ASTRoot* parse(const std::string&& source,const std::string&& file_name) noexcept;

const std::string toJson(const ASTRoot* root,bool pretty_mode) noexcept;

void enableSoftErrors() noexcept;
bool hasErrors() noexcept;
const std::string popSoftErrorOffStack() noexcept;
void disableSoftErrors() noexcept;