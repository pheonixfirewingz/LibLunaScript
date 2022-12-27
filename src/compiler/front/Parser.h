#pragma once
#include "../AST.h"

const ASTRoot* parse(const std::string&& source) noexcept;

const std::string toJson(const ASTRoot* root,bool pretty_mode) noexcept;

void enableSoftErrors() noexcept;
bool hasErrors() noexcept;
const char* popSoftErrorOffStack() noexcept;
void disableSoftErrors() noexcept;