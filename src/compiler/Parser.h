#pragma once
#include "Lexer.h"
#include "AST.h"
#include <rapidjson/prettywriter.h>
#include <rapidjson/stringbuffer.h>

const ASTRoot* parse(const std::vector<lexToken>&& tokens) noexcept;

const std::string toJson(const ASTRoot* root,bool pretty_mode) noexcept;

void enableSoftErrors() noexcept;
bool hasErrors() noexcept;
const char* popSoftErrorOffStack() noexcept;
void disableSoftErrors() noexcept;