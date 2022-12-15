#include "Parser.h"
#include <stack>
#include <stdio.h>

static std::stack<const char *> *gf_error_stack;

static inline bool isValidName(const lexToken &token) noexcept
{
    if (token.token != T_IDENTIFIER)
        return false;
    return true;
}

static inline bool isWhiteSpace(const lexToken &token) noexcept
{
    if (token.token != T_WHITESPACE)
        return false;
    return true;
}

static inline void eatWhitespace(const std::vector<lexToken> &lexer, uint32_t *i) noexcept
{
    while (isWhiteSpace(lexer[std::min<uint32_t>(lexer.size(), *i)]) && *i != lexer.size())
    {
        const uint32_t n = ++(*i);
        *i = std::min<uint32_t>(lexer.size(), n);
    }
}

static inline bool isDataType(const lexToken &token) noexcept
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

static inline bool isInteger(const lexToken &token) noexcept
{
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

static inline ASTDataType parseDataType(const lexToken &token) noexcept
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
        return NOT_DETERMINED_TYPE;
    }
}

void warn(const lexToken &, const char *msg_text) noexcept
{
    puts(msg_text);
}

void error(const lexToken &, const char *msg_text) noexcept
{
    if (gf_error_stack)
        gf_error_stack->push(msg_text);
    else
    {
        puts(msg_text);
        exit(1);
    }
}

ASTLiteral *parseLiteral(const std::vector<lexToken> &lexer, uint32_t *i) noexcept
{
    ASTLiteral *literal = new (std::nothrow) ASTLiteral();
    try
    {
        if (lexer[(*i)].token != T_QUOTE)
        {
            [[unlikely]] if (!isInteger(lexer[(*i)]))
                error(lexer[(*i)], "this is not a valid numeric value");
            literal->value = std::move(lexer[(*i)++].str_token);
            if (lexer[(*i)].token == T_DOT)
            {
                (*i)++;
                if (!isInteger(lexer[(*i)]))
                    error(lexer[(*i)], "this is not a valid numeric value");
                literal->value += '.';
                literal->value += std::move(lexer[(*i)++].str_token);
            }
            else if (literal->data_type == FLOAT32_TYPE || literal->data_type == FLOAT64_TYPE)
            {
                literal->value += ".0";
            }
        }
        else
            error(lexer[(*i)], "we don't have string support yet");
    }
    catch (std::exception &e)
    {
        error(lexer[(*i)], e.what());
        return nullptr;
    }
    return std::move(literal);
}

const ASTExpression *parseVar(const std::vector<lexToken> &lexer, uint32_t *i) noexcept
{
    ASTExpression *node = new (std::nothrow) ASTExpression();
    try
    {
        node->type = ASTE_VAR_DEFINED;
        ASTLiteral *literal;
        ASTDataType data_type;
        [[unlikely]] if (!isDataType(lexer[*i]))
        {
            error(lexer[*i], "this is not a supported data type");
            return node;
        }
        else 
            data_type = parseDataType(lexer[(*i)++]);
        eatWhitespace(lexer, i);
        [[unlikely]] if (!isValidName(lexer[*i]))
            error(lexer[*i], "this is not a valid param name");

        node->extra_data = std::move(lexer[(*i)++].str_token);
        eatWhitespace(lexer, i);
        if (lexer[(*i)].token != T_EQUAL)
        {
            eatWhitespace(lexer, i);
            literal = new (std::nothrow) ASTLiteral();
            literal->data_type = data_type;
            if (literal->data_type >= INT8_TYPE && literal->data_type <= UINT64_TYPE)
                literal->value = "0";
            else if (literal->data_type >= FLOAT32_TYPE && literal->data_type <= FLOAT64_TYPE)
                literal->value = "0.0";
            else
                literal->value = "";
        }
        else
        {
            (*i)++;
            eatWhitespace(lexer, i);
            literal = parseLiteral(lexer, i);
            [[unlikely]] if (!literal)
                literal = new (std::nothrow) ASTLiteral();
            literal->data_type = data_type;
        }
        node->list.emplace_back(std::move(literal));
    }
    catch (std::exception &e)
    {
        error(lexer[(*i)], e.what());
        return nullptr;
    }
    return std::move(node);
}

const ASTExpression *parseArgs(const std::vector<lexToken> &lexer, uint32_t *i) noexcept
{
    ASTExpression *expression = new (std::nothrow) ASTExpression();
    expression->type = ASTE_PRAM_LIST;
    eatWhitespace(lexer, i);
    if (lexer[*i].token == T_L_CURLY)
        (*i)++;
    if (lexer[*i].token == T_R_CURLY)
    {
        (*i)++;
        return expression;
    }
    uint32_t n = 0;
    do
    {
        [[unlikely]] if (n <= 100)
        {
            error(lexer[*i], "too many arguments or bad syntax");
            break;
        }

        [[unlikely]] if (!isDataType(lexer[*i]))
            error(lexer[*i], "this is not a supported data type");
        eatWhitespace(lexer, i);
        ASTLiteral *literal = new (std::nothrow) ASTLiteral();
        literal->data_type = parseDataType(lexer[(*i)++]);

        [[unlikely]] if (!isValidName(lexer[*i]))
            error(lexer[*i], "this is not a valid param name");

        literal->value = std::move(lexer[(*i)++].str_token);

        expression->list.emplace_back(std::move(literal));

        if (lexer[*i].token == T_COMMA)
        {
            (*i)++;
            eatWhitespace(lexer, i);
            continue;
        }
        else
            break;
    } while (true);
    return std::move(expression);
}

const ASTBlock *parseBlock(const std::vector<lexToken> &lexer, uint32_t *i) noexcept
{
    ASTBlock *block = new (std::nothrow) ASTBlock();
    (*i)++;
    eatWhitespace(lexer, i);
    do
    {
        if (lexer.size() <= (*i))
        {
            error(lexer[lexer.size() - 1], "there was no was to know when a block ended");
            break;
        }

        if (lexer[(*i)].token == T_RET)
        {
            (*i)++;
            ASTExpression *node = new (std::nothrow) ASTExpression();
            node->type = ASTE_RETURN;
            eatWhitespace(lexer, i);
            [[likely]] if (isInteger(lexer[(*i)]))
                node->list.emplace_back(parseLiteral(lexer, i));
            else
                error(lexer[(*i)], "unsupported return expression data");
            block->list.emplace_back(std::move(node));
        }
        else if (isDataType(lexer[(*i)]))
            block->list.emplace_back(parseVar(lexer, i));
        else
            error(lexer[(*i)++], "this is not a valid in this scope");
        eatWhitespace(lexer, i);
    } while (lexer[*i].token != T_R_SQUIGGLY);
    return std::move(block);
}

const ASTRoot *parse(const std::vector<lexToken> &&in) noexcept
{
    const std::vector<lexToken> lexer = in;
    ASTRoot *root = new (std::nothrow) ASTRoot();
    for (uint32_t i = 0; i < lexer.size(); ++i)
    {
        eatWhitespace(lexer, &i);
        if (lexer[i].token == T_FUNC)
        {
            i++;
            eatWhitespace(lexer, &i);
            [[unlikely]] if (!isValidName(lexer[i]))
                error(lexer[i], "this is not a valid function name");
            ASTFuncDef *node = new (std::nothrow) ASTFuncDef(std::move(lexer[i++].str_token));
            if (lexer[i].token != T_L_CURLY)
                error(lexer[i++], "this is not a valid function args opener");
            else
                node->args = parseArgs(lexer, &i);
            eatWhitespace(lexer, &i);
            if (lexer[i].token != T_S_ARROW)
            {
                node->return_type = VOID_TYPE;
                if (isDataType(lexer[i]))
                    error(lexer[i++], "you forgot to declare the return type -> to tell me what you are returning");
                else if (lexer[i].token != T_L_SQUIGGLY)
                    error(lexer[i++], "syntax error garbage return type");
            }
            else
            {
                i++;
                eatWhitespace(lexer, &i);
                if (!isDataType(lexer[i]))
                    error(lexer[i++], "this is not a supported data type");
                else
                    node->return_type = parseDataType(lexer[i++]);
            }
            eatWhitespace(lexer, &i);
            if (lexer[i].token != T_L_SQUIGGLY)
                error(lexer[i], "this is not a valid function block opener");
            else
                node->body = parseBlock(lexer, &i);
            root->children.emplace_back(std::move(node));
        }
        else if (isDataType(lexer[i]))
            root->children.emplace_back(parseVar(lexer, &i));
        else
        {
            error(lexer[i], "this is not a valid in global scope");
            return new (std::nothrow) ASTRoot();
        }
    }
    return root;
}

void enableSoftErrors() noexcept
{
    if (!gf_error_stack)
        gf_error_stack = new (std::nothrow) std::stack<const char *>();
}

bool hasErrors() noexcept
{
    if (gf_error_stack)
        return !gf_error_stack->empty();
    return false;
}

const char *popSoftErrorOffStack() noexcept
{
    const char *stack = gf_error_stack->top();
    gf_error_stack->pop();
    return stack;
}

void disableSoftErrors() noexcept
{
    if (gf_error_stack)
        delete gf_error_stack;
    gf_error_stack = nullptr;
}