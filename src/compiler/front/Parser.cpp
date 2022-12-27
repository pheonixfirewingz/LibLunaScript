#include "Parser.h"
#include "Lexer.h"
#include <new>
#include <stack>
#include <stdio.h>
#include <string>
#include <tuple>
#include <vector>

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

static inline const std::tuple<uint8_t, ASTOperatorType> parseOPType(const lexToken &token) noexcept
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
        return NOT_DETERMINED_DATA_TYPE;
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
            [[unlikely]] if (!isInteger(lexer[(*i)]) && !isValidName(lexer[(*i)]))
                error(lexer[(*i)], "this is not a valid numeric value or variable name");
            literal->value = std::move(lexer[(*i)++].str_token);
            if (lexer[(*i)].token == T_DOT)
            {
                (*i)++;
                [[unlikely]] if (!isInteger(lexer[(*i)]) && !isValidName(lexer[(*i)]))
                    error(lexer[(*i)], "this is not a valid numeric value or variable name");
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
        return literal;
    }
    return std::move(literal);
}

const ASTBinaryExpression *parseBinaryExpr(const uint8_t precedence_inflator, const std::vector<lexToken> &lexer,
                                           uint32_t *i) noexcept
{
    eatWhitespace(lexer, i);
    ASTBinaryExpression *expr = new (std::nothrow) ASTBinaryExpression();
    ASTLiteral *lit_1 = parseLiteral(lexer, i);
    if (lexer[(*i) - 2].token == T_DOT)
        lit_1->data_type = FLOAT_ANY_TYPE;
    else if (lexer[(*i) - 1].token == T_IDENTIFIER && !isInteger(lexer[(*i) - 1]))
        lit_1->data_type = NOT_DETERMINED_DATA_TYPE;
    else
        lit_1->data_type = UINT_ANY_TYPE;
    expr->left = std::move(lit_1);
    eatWhitespace(lexer, i);
    const std::tuple<uint16_t, ASTOperatorType> type = parseOPType(lexer[(*i)++]);
    expr->op = std::get<1>(type);
    expr->precedence = std::get<0>(type) + precedence_inflator;
    eatWhitespace(lexer, i);
    uint32_t fake_i = std::min<uint32_t>((*i) + 1, lexer.size());
    if (lexer[fake_i].token == T_DOT)
        fake_i += 2;
    eatWhitespace(lexer, &fake_i);
    if (isBinaryExpr(lexer[fake_i]))
    {
        const ASTBinaryExpression *r_expr = parseBinaryExpr(1 + precedence_inflator, lexer, i);
        [[unlikely]] if (!r_expr)
            r_expr = new (std::nothrow) ASTBinaryExpression();
        expr->right = std::move(r_expr);
    }
    else
    {
        ASTLiteral *lit_2 = parseLiteral(lexer, i);
        if (lexer[(*i) - 2].token == T_DOT)
            lit_2->data_type = FLOAT_ANY_TYPE;
        else if (lexer[(*i) - 1].token == T_IDENTIFIER && !isInteger(lexer[(*i) - 1]))
            lit_2->data_type = NOT_DETERMINED_DATA_TYPE;
        else
            lit_2->data_type = UINT_ANY_TYPE;
        expr->right = std::move(lit_2);
    }
    return expr;
}

const ASTExpression *parseVar(const std::vector<lexToken> &lexer, uint32_t *i) noexcept
{
    ASTExpression *node = new (std::nothrow) ASTExpression();
    node->type = ASTE_VAR_DEFINED;
    ASTDataType data_type;
    [[unlikely]] if (!isDataType(lexer[*i]))
    {
        error(lexer[*i], "this is not a supported data type");
        return node;
    }

    data_type = parseDataType(lexer[(*i)++]);
    eatWhitespace(lexer, i);
    [[unlikely]] if (!isValidName(lexer[*i]))
        error(lexer[*i], "this is not a valid param name");

    node->extra_data = std::move(lexer[(*i)++].str_token);
    eatWhitespace(lexer, i);
    if (lexer[(*i)].token != T_EQUAL)
    {
        eatWhitespace(lexer, i);
        if (isInteger(lexer[(*i)]))
            error(lexer[(*i)], "you missing the equals(=) to define value of the variable");
        ASTLiteral *literal = new (std::nothrow) ASTLiteral();
        literal->data_type = data_type;
        if (literal->data_type >= INT8_TYPE && literal->data_type <= UINT64_TYPE)
            literal->value = "0";
        else if (literal->data_type >= FLOAT32_TYPE && literal->data_type <= FLOAT64_TYPE)
            literal->value = "0.0";
        else
            literal->value = "";
        node->list.emplace_back(std::move(literal));
    }
    else
    {
        (*i)++;
        eatWhitespace(lexer, i);
        uint32_t fake_i = *i;
        fake_i++;
        if (lexer[fake_i].token == T_DOT)
            fake_i += 2;
        eatWhitespace(lexer, &fake_i);
        if (isBinaryExpr(lexer[fake_i]))
        {
            const ASTBinaryExpression *expr = parseBinaryExpr(0, lexer, i);
            [[unlikely]] if (!expr)
                expr = new (std::nothrow) ASTBinaryExpression();
            node->list.emplace_back(std::move(expr));
        }
        else
        {
            ASTLiteral *literal = parseLiteral(lexer, i);
            [[unlikely]] if (!literal)
                literal = new (std::nothrow) ASTLiteral();
            literal->data_type = std::move(data_type);
            node->list.emplace_back(std::move(literal));
        }
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
            uint32_t fake_i = *i;
            fake_i++;
            if (lexer[fake_i].token == T_DOT)
                fake_i += 2;
            eatWhitespace(lexer, &fake_i);
            if (isBinaryExpr(lexer[fake_i]))
            {
                const ASTBinaryExpression *expr = parseBinaryExpr(0, lexer, i);
                [[unlikely]] if (!expr)
                    expr = new (std::nothrow) ASTBinaryExpression();
                node->list.emplace_back(std::move(expr));
            }
            else
            {
                ASTLiteral *literal = parseLiteral(lexer, i);
                [[unlikely]] if (!literal)
                    literal = new (std::nothrow) ASTLiteral();
                if (lexer[(*i) - 2].token == T_DOT)
                    literal->data_type = FLOAT_ANY_TYPE;
                else if (lexer[(*i) - 1].token == T_IDENTIFIER && !isInteger(lexer[(*i) - 1]))
                    literal->data_type = NOT_DETERMINED_DATA_TYPE;
                else
                    literal->data_type = UINT_ANY_TYPE;
                node->list.emplace_back(std::move(literal));
            }
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

const ASTRoot *iParse(const std::vector<lexToken> &&in) noexcept
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

const ASTRoot *parse(const std::string&& source) noexcept
{
    return iParse(lexIt(std::move(source)));
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