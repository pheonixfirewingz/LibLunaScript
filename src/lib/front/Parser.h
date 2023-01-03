#pragma once
#include "../AST.h"
#include <extend_std/LookUpTable.h>
#include <regex>
#include <stack>
#include <stdio.h>
#include <string>
#include <tuple>
#include <vector>

namespace LunaScript::compiler::front
{

class Parser
{
    std::stack<std::string> *gf_error_stack;
    ASTRoot *root;
    // this public section is stupid but for speed / optimizations sake init tables for ever ast create would be a
    // nightmare
  public:
    typedef enum LexTokenEnum : uint8_t
    {
        T_ERROR_BAD_TOKEN,
        T_IDENTIFIER,
        T_FUNC,
        T_PUBLIC_FUNC,
        T_RET,
        T_R_SQUIGGLY,
        T_L_SQUIGGLY,
        T_R_CURLY,
        T_L_CURLY,
        T_COMMA,
        T_DOT,
        T_ADD,
        T_SUB,
        T_MUL,
        T_DIV,
        T_AND,
        T_OR,
        T_XOR,
        T_MODULO,
        T_EQUAL,
        T_R_ARROW,
        T_L_ARROW,
        T_S_ARROW,
        T_QUOTE,
        T_SINGLE_LETTER_QUOTE,
        // types
        T_NO_RETURN,
        T_INT8,
        T_INT16,
        T_INT32,
        T_INT64,
        T_UINT8,
        T_UINT16,
        T_UINT32,
        T_UINT64,
        T_FLOAT32,
        T_FLOAT64,
        T_STRING,
    } LexTokenEnum;

    typedef struct lexToken
    {
        LexTokenEnum token;
        std::string str_token = "";
        uint32_t line = 0;
        explicit lexToken() = default;
        lexToken(LexTokenEnum in_tok, uint32_t int_in, std::string str_token) noexcept
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
    } lexToken;

  private:
    uint32_t lexer_index = 0;
    inline uint32_t getIndexGuard(const std::vector<lexToken> &lexer, bool inc = false) noexcept
    {
        if (inc && lexer_index < lexer.size())
            return lexer_index++;
        return lexer_index;
    }

    inline bool isValidName(const lexToken &token) const noexcept
    {
        if (token.token != T_IDENTIFIER)
            return false;
        return true;
    }

    inline bool isValidFuncCall(const std::vector<lexToken> &lexer) noexcept
    {
        if (lexer[getIndexGuard(lexer)].token == T_IDENTIFIER && lexer[getIndexGuard(lexer) + 1].token == T_L_CURLY)
            return true;
        return false;
    }

    inline bool isDataType(const lexToken &token) const noexcept
    {
        switch (token.token)
        {
        case T_INT8:
        case T_UINT8:
        case T_INT16:
        case T_UINT16:
        case T_INT32:
        case T_UINT32:
        case T_INT64:
        case T_UINT64:
        case T_FLOAT32:
        case T_FLOAT64:
        case T_STRING:
            return true;
        [[unlikely]] default:
            return false;
        }
    }

    static inline bool isBinaryExpr(const lexToken &token) noexcept
    {
        switch (token.token)
        {
        case T_ADD:
        case T_SUB:
        case T_MUL:
        case T_DIV:
        case T_AND:
        case T_OR:
        case T_XOR:
        case T_MODULO:
            return true;
        default:
            return false;
        }
    }

    inline bool isInteger(const lexToken &token) const noexcept
    {
        if (token.token == T_ERROR_BAD_TOKEN)
            return false;
        switch (token.token)
        {
        case T_IDENTIFIER: {
            std::string::const_iterator it = token.str_token.begin();
            while (it != token.str_token.end() && std::isdigit(*it))
                ++it;
            return !token.str_token.empty() && it == token.str_token.end();
        }
        case T_INT8:
        case T_UINT8:
        case T_INT16:
        case T_UINT16:
        case T_INT32:
        case T_UINT32:
        case T_INT64:
        case T_UINT64:
            return true;
        case T_FLOAT32:
        case T_FLOAT64:
        case T_STRING:
        default:
            return false;
        }
    }

    inline const std::tuple<uint8_t, ASTOperatorType> parseOPType(const lexToken &token) const noexcept
    {
        switch (token.token)
        {
        case T_ADD:
            return std::make_tuple<uint8_t, ASTOperatorType>(5, ADD_TYPE);
        case T_SUB:
            return std::make_tuple<uint8_t, ASTOperatorType>(5, SUB_TYPE);
        case T_MUL:
            return std::make_tuple<uint8_t, ASTOperatorType>(6, MUL_TYPE);
        case T_DIV:
            return std::make_tuple<uint8_t, ASTOperatorType>(6, DIV_TYPE);
        case T_AND:
            return std::make_tuple<uint8_t, ASTOperatorType>(1, AND_TYPE);
        case T_OR:
            return std::make_tuple<uint8_t, ASTOperatorType>(1, OR_TYPE);
        case T_XOR:
            return std::make_tuple<uint8_t, ASTOperatorType>(1, XOR_TYPE);
        case T_MODULO:
            return std::make_tuple<uint8_t, ASTOperatorType>(6, MOD_TYPE);
        [[unlikely]] default:
            return std::make_tuple<uint8_t, ASTOperatorType>(0, NOT_DETERMINED_OP_TYPE);
        }
    }

    inline ASTDataType parseDataType(const lexToken &token) const noexcept
    {
        switch (token.token)
        {
        case T_INT8:
            return INT8_TYPE;
        case T_UINT8:
            return UINT8_TYPE;
        case T_INT16:
            return INT16_TYPE;
        case T_UINT16:
            return UINT16_TYPE;
        case T_INT32:
            return INT32_TYPE;
        case T_UINT32:
            return UINT32_TYPE;
        case T_INT64:
            return INT64_TYPE;
        case T_UINT64:
            return UINT64_TYPE;
        case T_FLOAT32:
            return FLOAT32_TYPE;
        case T_FLOAT64:
            return FLOAT64_TYPE;
        [[unlikely]] default:
            return NOT_DETERMINED_DATA_TYPE;
        }
    }

    inline const char *getTokenName(LexTokenEnum tok) const noexcept
    {
        switch (tok)
        {
        case T_IDENTIFIER:
            return "identifier";
        case T_FUNC:
            return "func";
        case T_PUBLIC_FUNC:
            return "public";
        case T_RET:
            return "return";
        case T_R_SQUIGGLY:
            return "{";
        case T_L_SQUIGGLY:
            return "}";
        case T_R_CURLY:
            return "(";
        case T_L_CURLY:
            return ")";
        case T_COMMA:
            return ",";
        case T_DOT:
            return ".";
        case T_ADD:
            return "+";
        case T_SUB:
            return "-";
        case T_MUL:
            return "*";
        case T_DIV:
            return "/";
        case T_AND:
            return "and";
        case T_OR:
            return "or";
        case T_XOR:
            return "xor";
        case T_MODULO:
            return "%";
        case T_EQUAL:
            return "=";
        case T_R_ARROW:
            return ">";
        case T_L_ARROW:
            return "<";
        case T_S_ARROW:
            return "->";
        case T_QUOTE:
            return "\"";
        case T_SINGLE_LETTER_QUOTE:
            return "\'";
        case T_NO_RETURN:
            return "void";
        case T_INT8:
            return "int8";
        case T_INT16:
            return "int16";
        case T_INT32:
            return "int32";
        case T_INT64:
            return "int64";
        case T_UINT8:
            return "uint8";
        case T_UINT16:
            return "uint16";
        case T_UINT32:
            return "uint32";
        case T_UINT64:
            return "uint64";
        case T_STRING:
            return "string";
        case T_FLOAT32:
            return "float32";
        case T_FLOAT64:
            return "float64";
        default:
            return "unknown";
        }
    }

    void warn(const lexToken &token, const char *msg_text) noexcept
    {
        std::string name;
        if (token.token != T_IDENTIFIER)
            name = getTokenName(token.token);
        else
            name = std::string(token.str_token);
        std::string out = msg_text;
        out.resize(out.size() + name.size() + 29);
        sprintf(out.data(), "%s:Line %u -> Warning Msg: %s", name.c_str(), token.line, msg_text);
        puts(out.c_str());
    }

    void error(const lexToken &token, const char *msg_text) noexcept
    {
        if (gf_error_stack)
        {
            std::string name;
            if (token.token != T_IDENTIFIER)
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
            if (token.token != T_IDENTIFIER)
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

    ASTLiteral *parseLiteral(const std::vector<lexToken> &lexer) noexcept;
    const ASTBinaryExpression *parseBinaryExpr(const uint8_t precedence_inflator,
                                               const std::vector<lexToken> &lexer) noexcept;
    const ASTExpression *parseVar(const std::vector<lexToken> &lexer, bool is_global = false) noexcept;
    const ASTExpression *parseArgs(const std::vector<lexToken> &lexer) noexcept;
    ASTBlock *parseBlock(const std::vector<lexToken> &lexer) noexcept;
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