#pragma once
#include "../AST.h"
#include <extend_std/LookUpTable.h>
#include <extend_std/Vector.h>
#include <inttypes.h>
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

    inline void replace_all(std::string &inout, std::string_view what, std::string_view with)
    {
        std::size_t count{};
        for (std::string::size_type pos{}; inout.npos != (pos = inout.find(what.data(), pos, what.length()));
             pos += with.length(), ++count)
            inout.replace(pos, what.length(), with.data(), with.length());
    }

    inline char tolower(char in)
    {
        if (in <= 'Z' && in >= 'A')
            return in - ('Z' - 'z');
        return in;
    }

    // this public section is stupid but for speed / optimizations sake init tables for ever ast create would be a
    // nightmare
  public:
    enum class LexToken : uint8_t
    {
        ERROR,
        IMPORT,
        MODULE_NAME,
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
        GLOBAL,
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
        size_t line = 0;
        lexToken() = default;
        lexToken(LexToken in_tok, size_t int_in, std::string str_token) noexcept
            : token(in_tok)
            , str_token(str_token)
            , line(int_in)
        {
        }

        inline constexpr bool operator!=(const lexToken tok) const
        {
            return token != tok.token && line != tok.line;
        }

        inline constexpr bool operator==(const lexToken tok) const
        {
            return token == tok.token && line == tok.line;
        }
    };

  private:
    size_t lexer_index = 0;
    std::vector<std::string> last_modules;

    inline size_t getIndexGuard(const std::vector<lexToken> &lexer, bool inc = false) noexcept
    {
        if (inc && lexer_index < (lexer.size() - 1))
            return lexer_index++;
        return lexer_index;
    }

    inline constexpr bool isValidName(const lexToken &token) const noexcept
    {
        return token.token == LexToken::IDENTIFIER;
    }

    inline bool isValidFuncCall(const std::vector<lexToken> &lexer) noexcept
    {
        if (lexer_index + 1 < (lexer.size() - 1))
        {
            if (lexer[getIndexGuard(lexer)].token == LexToken::IDENTIFIER &&
                lexer[(uint64_t)getIndexGuard(lexer) + 1].token == LexToken::L_CURLY)
                return true;
        }
        return false;
    }

    inline constexpr bool isDataType(const lexToken &token) const noexcept
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
            return "<";
        case LexToken::L_ARROW:
            return ">";
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
        token.token != LexToken::IDENTIFIER ? name = getTokenName(token.token) : name = std::string(token.str_token);
        std::string out = msg_text;
        out.resize(out.size() + name.size() + 29);
#if IS_MSVC
#    pragma warning(push)
#    pragma warning(disable : 4996)
#endif
        sprintf(out.data(), "%s:Line %" PRId64 " -> Warning Msg: %s", name.c_str(), token.line, msg_text);
#if IS_MSVC
#    pragma warning(pop)
#endif
        puts(out.c_str());
    }

    uint8_t error_count = 0;
    void error(const lexToken &token, const char *msg_text)
    {
        error_count >= 20 ? throw std::runtime_error("Too many errors") : error_count++;
        std::string name;
        token.token != LexToken::IDENTIFIER ? name = getTokenName(token.token) : name = std::string(token.str_token);
        std::string out = msg_text;
        out.resize(out.size() + name.size() + 29);
#if IS_MSVC
#    pragma warning(push)
#    pragma warning(disable : 4996)
#endif
        sprintf(out.data(), "[%s]:Line %" PRId64 " -> Error Msg: %s", name.c_str(), token.line, msg_text);
#if IS_MSVC
#    pragma warning(pop)
#endif
        gf_error_stack->push(out);
    }

    ASTLiteral *parseLiteral(const std::vector<lexToken> &lexer);
    const ASTBinaryExpression *parseBinaryExpr(const uint8_t precedence_inflator, const std::vector<lexToken> &lexer);
    const ASTVarDef *parseVar(const std::vector<lexToken> &lexer, bool is_global = false);
    ASTParamListExpression *parseArgs(const std::vector<lexToken> &lexer,const bool call_mode = false);
    ASTBlock *parseBlock(const std::vector<lexToken> &lexer);
    ASTModule *parseModule(const std::vector<lexToken> &&lexer, const std::string module_name);
    void parse(const std::string &&source, bool debug) noexcept;

  public:
    explicit Parser(const std::string &&source, bool debug) noexcept
        : gf_error_stack(new std::stack<std::string>())
        , root(new ASTRoot())
    {
        parse((const std::string &&)source, debug);
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

    std::string colourSrcText(std::string text)
    {
        static const char *const src_words[] = {"Import",  "Module",  "Global", "Const", "Uint8", "Uint16",
                                                "Uint32",  "Uint64",  "Int8",   "Int16", "Int32", "Int64",
                                                "Float32", "Float64", "Ret",    "Func",  "Public"};
        for (uint16_t i = 0; i < sizeof(src_words) / sizeof(const char *); i++)
        {
            std::string word(src_words[i]);
            std::string e("\x1B[94m");
            e += word;
            e += "\x1B[00m";
            std::string t;
            t += tolower(word[0]);
            t += std::string(word.c_str(), 1, word.size());
            replace_all(text, t, e);
            replace_all(text, word, t);
        }
        return text;
    }
};
} // namespace LunaScript::compiler::front