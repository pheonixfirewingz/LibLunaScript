#pragma once
#include "../AST.h"
#include <extend_std/LookUpTable.h>
#include <libos/Defines.h>
#include <regex>
#include <stack>
#include <stdio.h>
#include <string>
#include <tuple>
#include <vector>

namespace LunaScript::compiler::front
{
using namespace LunaScript::compiler::ast;
class Parser
{
    std::stack<std::string> *gf_error_stack;
    ASTRoot *root;
    // this public section is stupid but for speed / optimizations sake init tables for ever ast create would be a
    // nightmare
  public:
    enum class LexToken : uint8_t
    {
        ERROR,
        IDENTIFIER,
        FUNC,
        PUBLIC_FUNC,
        RET,
        R_SQUIGGLY,
        L_SQUIGGLY,
        R_CURLY,
        L_CURLY,
        COMMA,
        DOT,
        ADD,
        SUB,
        MUL,
        DIV,
        AND,
        OR,
        XOR,
        MODULO,
        EQUAL,
        R_ARROW,
        L_ARROW,
        S_ARROW,
        QUOTE,
        SINGLE_LETTER_QUOTE,
        // types
        NO_RETURN,
        INT8,
        INT16,
        INT32,
        INT64,
        UINT8,
        UINT16,
        UINT32,
        UINT64,
        FLOAT32,
        FLOAT64,
        STRING,
    };

    struct lexToken
    {
        LexToken token = LexToken::ERROR;
        std::string str_token = "";
        uint32_t line = 0;
        lexToken() = default;
        lexToken(LexToken in_tok, uint32_t int_in, std::string str_token) noexcept
            : token(in_tok)
            , str_token(str_token)
            , line(int_in)
        {
        }

        inline bool operator!=(const lexToken tok) const
        {
            return token != tok.token && line != tok.line;
        }

        inline bool operator==(const lexToken tok) const
        {
            return token == tok.token && line == tok.line;
        }
    };

  private:
    uint32_t lexer_index = 0;
    inline uint32_t getIndexGuard(const std::vector<lexToken> &lexer, bool inc = false) noexcept
    {
        if (inc && lexer_index < (lexer.size() - 1))
            return lexer_index++;
        return lexer_index;
    }

    inline bool isValidName(const lexToken &token) const noexcept
    {
        if (token.token != LexToken::IDENTIFIER)
            return false;
        return true;
    }

    inline bool isValidFuncCall(const std::vector<lexToken> &lexer) noexcept
    {
        if (lexer[getIndexGuard(lexer)].token == LexToken::IDENTIFIER &&
            lexer[(uint64_t)getIndexGuard(lexer) + 1].token == LexToken::L_CURLY)
            return true;
        return false;
    }

    inline bool isDataType(const lexToken &token) const noexcept
    {
        switch (token.token)
        {
        case LexToken::INT8:
        case LexToken::UINT8:
        case LexToken::INT16:
        case LexToken::UINT16:
        case LexToken::INT32:
        case LexToken::UINT32:
        case LexToken::INT64:
        case LexToken::UINT64:
        case LexToken::FLOAT32:
        case LexToken::FLOAT64:
        case LexToken::STRING:
            return true;
        [[unlikely]] default:
            return false;
        }
    }

    static inline bool isBinaryExpr(const lexToken &token) noexcept
    {
        switch (token.token)
        {
        case LexToken::ADD:
        case LexToken::SUB:
        case LexToken::MUL:
        case LexToken::DIV:
        case LexToken::AND:
        case LexToken::OR:
        case LexToken::XOR:
        case LexToken::MODULO:
            return true;
        default:
            return false;
        }
    }

    inline bool isInteger(const lexToken &token) const noexcept
    {
        switch (token.token)
        {
        case LexToken::IDENTIFIER: {
            std::string::const_iterator it = token.str_token.begin();
            while (it != token.str_token.end() && std::isdigit(*it))
                ++it;
            return !token.str_token.empty() && it == token.str_token.end();
        }
        case LexToken::INT8:
        case LexToken::UINT8:
        case LexToken::INT16:
        case LexToken::UINT16:
        case LexToken::INT32:
        case LexToken::UINT32:
        case LexToken::INT64:
        case LexToken::UINT64:
            return true;
        case LexToken::FLOAT32:
        case LexToken::FLOAT64:
        case LexToken::STRING:
        default:
            return false;
        }
    }

    inline const std::tuple<uint8_t, OperatorType> parseOPType(const lexToken &token) const noexcept
    {
        switch (token.token)
        {
        case LexToken::ADD:
            return std::make_tuple<uint8_t, OperatorType>(10, OperatorType::ADD);
        case LexToken::SUB:
            return std::make_tuple<uint8_t, OperatorType>(20, OperatorType::SUB);
        case LexToken::MUL:
            return std::make_tuple<uint8_t, OperatorType>(30, OperatorType::MUL);
        case LexToken::DIV:
            return std::make_tuple<uint8_t, OperatorType>(40, OperatorType::DIV);
        case LexToken::AND:
            return std::make_tuple<uint8_t, OperatorType>(60, OperatorType::AND);
        case LexToken::OR:
            return std::make_tuple<uint8_t, OperatorType>(70, OperatorType::OR);
        case LexToken::XOR:
            return std::make_tuple<uint8_t, OperatorType>(80, OperatorType::XOR);
        case LexToken::MODULO:
            return std::make_tuple<uint8_t, OperatorType>(50, OperatorType::MOD);
        [[unlikely]] default:
            return std::make_tuple<uint8_t, OperatorType>(0, OperatorType::NOT_DETERMINED);
        }
    }

    inline DataType parseDataType(const lexToken &token) const noexcept
    {
        switch (token.token)
        {
        case LexToken::INT8:
            return DataType::INT8;
        case LexToken::UINT8:
            return DataType::UINT8;
        case LexToken::INT16:
            return DataType::INT16;
        case LexToken::UINT16:
            return DataType::UINT16;
        case LexToken::INT32:
            return DataType::INT32;
        case LexToken::UINT32:
            return DataType::UINT32;
        case LexToken::INT64:
            return DataType::INT64;
        case LexToken::UINT64:
            return DataType::UINT64;
        case LexToken::FLOAT32:
            return DataType::FLOAT32;
        case LexToken::FLOAT64:
            return DataType::FLOAT64;
        [[unlikely]] default:
            return DataType::NOT_DETERMINED;
        }
    }

    inline const char *getTokenName(LexToken tok) const noexcept
    {
        switch (tok)
        {
        case LexToken::IDENTIFIER:
            return "identifier";
        case LexToken::FUNC:
            return "func";
        case LexToken::PUBLIC_FUNC:
            return "public";
        case LexToken::RET:
            return "return";
        case LexToken::R_SQUIGGLY:
            return "{";
        case LexToken::L_SQUIGGLY:
            return "}";
        case LexToken::R_CURLY:
            return "(";
        case LexToken::L_CURLY:
            return ")";
        case LexToken::COMMA:
            return ",";
        case LexToken::DOT:
            return ".";
        case LexToken::ADD:
            return "+";
        case LexToken::SUB:
            return "-";
        case LexToken::MUL:
            return "*";
        case LexToken::DIV:
            return "/";
        case LexToken::AND:
            return "and";
        case LexToken::OR:
            return "or";
        case LexToken::XOR:
            return "xor";
        case LexToken::MODULO:
            return "%";
        case LexToken::EQUAL:
            return "=";
        case LexToken::R_ARROW:
            return ">";
        case LexToken::L_ARROW:
            return "<";
        case LexToken::S_ARROW:
            return "->";
        case LexToken::QUOTE:
            return "\"";
        case LexToken::SINGLE_LETTER_QUOTE:
            return "\'";
        case LexToken::NO_RETURN:
            return "void";
        case LexToken::INT8:
            return "int8";
        case LexToken::INT16:
            return "int16";
        case LexToken::INT32:
            return "int32";
        case LexToken::INT64:
            return "int64";
        case LexToken::UINT8:
            return "uint8";
        case LexToken::UINT16:
            return "uint16";
        case LexToken::UINT32:
            return "uint32";
        case LexToken::UINT64:
            return "uint64";
        case LexToken::STRING:
            return "string";
        case LexToken::FLOAT32:
            return "float32";
        case LexToken::FLOAT64:
            return "float64";
        default:
            return "unknown";
        }
    }

    void warn(const lexToken &token, const char *msg_text) noexcept
    {
        std::string name;
        if (token.token != LexToken::IDENTIFIER)
            name = getTokenName(token.token);
        else
            name = std::string(token.str_token);
        std::string out = msg_text;
        out.resize(out.size() + name.size() + 29);
        sprintf(out.data(), "%s:Line %u -> Warning Msg: %s", name.c_str(), token.line, msg_text);
        puts(out.c_str());
    }

    uint8_t error_count = 0;
    void error(const lexToken &token, const char *msg_text)
    {
        if(error_count >= 20)
            throw std::runtime_error("Too many errors");
        else
            error_count++;

        if (gf_error_stack)
        {
            std::string name;
            if (token.token != LexToken::IDENTIFIER)
                name = getTokenName(token.token);
            else
                name = std::string(token.str_token);
            std::string out = msg_text;
            out.resize(out.size() + name.size() + 29);
            sprintf(out.data(), "[%s]:Line %u -> Error Msg: %s", name.c_str(), token.line, msg_text);
            gf_error_stack->push(out);
        }
        else
        {
            std::string name;
            if (token.token != LexToken::IDENTIFIER)
                name = getTokenName(token.token);
            else
                name = std::string(token.str_token);
            std::string out = msg_text;
            out.resize(out.size() + name.size() + 29);
            sprintf(out.data(), "[%s]:Line %u -> Error Msg: %s", name.c_str(), token.line, msg_text);
            puts(out.c_str());
            exit(1);
        }
    }

    ASTLiteral *parseLiteral(const std::vector<lexToken> &lexer);
    const ASTBinaryExpression *parseBinaryExpr(const uint8_t precedence_inflator,
                                               const std::vector<lexToken> &lexer);
    const ASTExpression *parseVar(const std::vector<lexToken> &lexer, bool is_global = false);
    const ASTExpression *parseArgs(const std::vector<lexToken> &lexer);
    ASTBlock *parseBlock(const std::vector<lexToken> &lexer);
    void parse(const std::string &&source) noexcept;

  public:
    explicit Parser(const std::string &&source, const std::string &&file_name) noexcept
        : gf_error_stack(new std::stack<std::string>())
        , root(new ASTRoot(file_name))
    {
        parse((const std::string &&)source);
    }

    bool hasErrors() const noexcept
    {
        return !gf_error_stack->empty();
    }

    const std::string popErrorOffStack() noexcept
    {
        std::string stack = gf_error_stack->top();
        gf_error_stack->pop();
        return stack;
    }

    const std::string asString(const bool pretty_mode) noexcept;
    const ASTRoot *getAST() const noexcept
    {
        return root;
    }

    Parser(const Parser &) = delete;
    Parser(Parser &&) = delete;
    Parser &operator=(const Parser &) = delete;
    Parser &operator=(Parser &&) = delete;

    ~Parser()
    {
        delete gf_error_stack;
        delete root;
    }
};
} // namespace LunaScript::compiler::front