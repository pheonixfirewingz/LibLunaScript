#include "Parser.h"
namespace LunaScript::compiler::front
{
static const std::ReadOnlyLookupTable<size_t, Parser::LexTokenEnum> lookup = {
    {std::hash<std::string>{}("void"), Parser::T_NO_RETURN},
    {std::hash<std::string>{}("int8"), Parser::T_INT8},
    {std::hash<std::string>{}("int16"), Parser::T_INT16},
    {std::hash<std::string>{}("int32"), Parser::T_INT32},
    {std::hash<std::string>{}("int64"), Parser::T_INT64},
    {std::hash<std::string>{}("uint8"), Parser::T_UINT8},
    {std::hash<std::string>{}("uint16"), Parser::T_UINT16},
    {std::hash<std::string>{}("uint32"), Parser::T_UINT32},
    {std::hash<std::string>{}("uint64"), Parser::T_UINT64},
    {std::hash<std::string>{}("float32"), Parser::T_FLOAT32},
    {std::hash<std::string>{}("float64"), Parser::T_FLOAT64},
    {std::hash<std::string>{}("func"), Parser::T_FUNC},
    {std::hash<std::string>{}("public"), Parser::T_PUBLIC_FUNC},
    {std::hash<std::string>{}("ret"), Parser::T_RET},
    {std::hash<std::string>{}("{"), Parser::T_L_SQUIGGLY},
    {std::hash<std::string>{}("}"), Parser::T_R_SQUIGGLY},
    {std::hash<std::string>{}("("), Parser::T_L_CURLY},
    {std::hash<std::string>{}(")"), Parser::T_R_CURLY},
    {std::hash<std::string>{}(","), Parser::T_COMMA},
    {std::hash<std::string>{}("."), Parser::T_DOT},
    {std::hash<std::string>{}("+"), Parser::T_ADD},
    {std::hash<std::string>{}("-"), Parser::T_SUB},
    {std::hash<std::string>{}("/"), Parser::T_DIV},
    {std::hash<std::string>{}("*"), Parser::T_MUL},
    {std::hash<std::string>{}("="), Parser::T_EQUAL},
    {std::hash<std::string>{}("and"), Parser::T_AND},
    {std::hash<std::string>{}("or"), Parser::T_OR},
    {std::hash<std::string>{}(">"), Parser::T_L_ARROW},
    {std::hash<std::string>{}("<"), Parser::T_R_ARROW},
    {std::hash<std::string>{}("mod"), Parser::T_MODULO},
    {std::hash<std::string>{}("\""), Parser::T_QUOTE},
    {std::hash<std::string>{}("\'"), Parser::T_SINGLE_LETTER_QUOTE},
    {std::hash<std::string>{}("xor"), Parser::T_XOR},
};

static const std::ReadOnlyLookupTable<size_t, bool> reject = {
    {std::hash<std::string>{}("\n"), true},
    {std::hash<std::string>{}("\r"), false},
    {std::hash<std::string>{}(" "), false},
    {std::hash<std::string>{}("\t"), false},
};
static const std::regex regex(
    "(([A-Za-z0-9-]+)|(\\s)|([\\\",\\\\',\\(,\\),\\[,\\],\\<,\\>,\\{,\\},\\,:,\\.])|([_,+,=,*,/]+))",
    std::regex_constants::icase);

ASTLiteral *Parser::parseLiteral(const std::vector<lexToken> &lexer) noexcept
{
    ASTLiteral *literal = new ASTLiteral();
    if (lexer[getIndexGuard(lexer)].token != T_QUOTE)
    {
        [[unlikely]] if (!isInteger(lexer[getIndexGuard(lexer)]) && !isValidName(lexer[getIndexGuard(lexer)]))
            error(lexer[getIndexGuard(lexer)], "this is not a valid numeric value or variable name");
        literal->value = std::move(lexer[getIndexGuard(lexer,true)].str_token);
        if (lexer[getIndexGuard(lexer)].token == T_DOT)
        {
            [[unlikely]] if (!isInteger(lexer[getIndexGuard(lexer,true)]) && !isValidName(lexer[getIndexGuard(lexer)]))
                error(lexer[getIndexGuard(lexer)], "this is not a valid numeric value or variable name");
            literal->value += '.';
            literal->value += std::move(lexer[getIndexGuard(lexer,true)].str_token);
        }
        else if (literal->data_type == FLOAT32_TYPE || literal->data_type == FLOAT64_TYPE)
        {
            literal->value += ".0";
        }
    }
    else
        error(lexer[getIndexGuard(lexer)], "we don't have string support yet");
    return std::move(literal);
}

const ASTBinaryExpression *Parser::parseBinaryExpr(const uint8_t precedence_inflator,const std::vector<lexToken> &lexer) noexcept
{
    ASTBinaryExpression *expr = new ASTBinaryExpression();
    ASTLiteral *lit_1 = parseLiteral(lexer);
    if (lexer[getIndexGuard(lexer) - 2].token == T_DOT)
        lit_1->data_type = FLOAT_ANY_TYPE;
    else if (lexer[getIndexGuard(lexer) - 1].token == T_IDENTIFIER && !isInteger(lexer[getIndexGuard(lexer) - 1]))
        lit_1->data_type = NOT_DETERMINED_DATA_TYPE;
    else
        lit_1->data_type = UINT_ANY_TYPE;
    expr->left = std::move(lit_1);
    const std::tuple<uint16_t, ASTOperatorType> type = parseOPType(lexer[getIndexGuard(lexer,true)]);
    expr->op = std::get<1>(type);
    expr->precedence = std::get<0>(type) + precedence_inflator;
    uint32_t fake_i = std::min<uint32_t>(getIndexGuard(lexer) + 1, lexer.size());
    if (lexer[fake_i].token == T_DOT)
        fake_i += 2;
    if (isBinaryExpr(lexer[fake_i]))
    {
        const ASTBinaryExpression *r_expr = parseBinaryExpr(1 + precedence_inflator, lexer);
        [[unlikely]] if (!r_expr)
            r_expr = new ASTBinaryExpression();
        expr->right = std::move(r_expr);
    }
    else
    {
        if (!isInteger(lexer[getIndexGuard(lexer)]) && !isValidName(lexer[getIndexGuard(lexer)]))
        {
            error(lexer[getIndexGuard(lexer)], "no good data or variable provided");
            return expr;
        }
        ASTLiteral *lit_2 = parseLiteral(lexer);
        if (lexer[getIndexGuard(lexer) - 2].token == T_DOT)
            lit_2->data_type = FLOAT_ANY_TYPE;
        else if (lexer[getIndexGuard(lexer) - 1].token == T_IDENTIFIER && !isInteger(lexer[getIndexGuard(lexer) - 1]))
            lit_2->data_type = NOT_DETERMINED_DATA_TYPE;
        else
            lit_2->data_type = UINT_ANY_TYPE;
        expr->right = std::move(lit_2);
    }
    return expr;
}

const ASTExpression *Parser::parseVar(const std::vector<lexToken> &lexer,bool is_global) noexcept
{
    ASTExpression *node = new ASTExpression();
    node->type = AST_EXPR_VAR_DEFINED;
    ASTDataType data_type;
    [[unlikely]] if (!isDataType(lexer[getIndexGuard(lexer)]))
    {
        error(lexer[getIndexGuard(lexer)], "this is not a supported data type");
        return node;
    }
    data_type = parseDataType(lexer[getIndexGuard(lexer,true)]);
    node->data_type = data_type;
    [[unlikely]] if (!isValidName(lexer[getIndexGuard(lexer)]))
        error(lexer[getIndexGuard(lexer)], "this is not a valid param name");
    node->extra_data = std::move(lexer[getIndexGuard(lexer,true)].str_token);
    if (lexer[getIndexGuard(lexer)].token != T_EQUAL)
    {
        if (isInteger(lexer[getIndexGuard(lexer)]))
            error(lexer[getIndexGuard(lexer)], "you missing the equals(=) to define value of the variable");
        ASTLiteral *literal = new ASTLiteral();
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
        (void)getIndexGuard(lexer,true);

        if (isValidFuncCall(lexer) && !is_global)
        {
            puts("var def: function call hit");
            std::exit(0);
        }
        if (!isInteger(lexer[getIndexGuard(lexer)]) && !isValidName(lexer[getIndexGuard(lexer)]))
        {
            error(lexer[getIndexGuard(lexer)], "no good stating data for variable provided");
            return node;
        }
        uint32_t fake_i = getIndexGuard(lexer);
        fake_i++;
        if (lexer[fake_i].token == T_DOT)
            fake_i += 2;
        if (isBinaryExpr(lexer[fake_i]))
        {
            const ASTBinaryExpression *expr = parseBinaryExpr(0, lexer);
            [[unlikely]] if (!expr)
                expr = new ASTBinaryExpression();
            node->list.emplace_back(std::move(expr));
        }
        else
        {
            ASTLiteral *literal = parseLiteral(lexer);
            [[unlikely]] if (!literal)
                literal = new ASTLiteral();
            literal->data_type = std::move(data_type);
            node->list.emplace_back(std::move(literal));
        }
    }
    return std::move(node);
}

const ASTExpression *Parser::parseArgs(const std::vector<lexToken> &lexer) noexcept
{
    ASTExpression *expression = new ASTExpression();
    expression->type = AST_EXPR_PRAM_LIST;
    if (lexer[getIndexGuard(lexer)].token == T_L_CURLY)
        (void)getIndexGuard(lexer,true);
    if (lexer[getIndexGuard(lexer)].token != T_R_CURLY)
    {
        uint32_t n = 0;
        do
        {
            [[unlikely]] if (n >= 100)
            {
                error(lexer[getIndexGuard(lexer)], "too many arguments or bad syntax");
                break;
            }
            [[unlikely]] if (!isDataType(lexer[getIndexGuard(lexer)]))
                error(lexer[getIndexGuard(lexer)], "this is not a supported data type");
            ASTLiteral *literal = new ASTLiteral();
            literal->data_type = parseDataType(lexer[getIndexGuard(lexer,true)]);
            [[unlikely]] if (!isValidName(lexer[getIndexGuard(lexer)]))
                error(lexer[getIndexGuard(lexer)], "this is not a valid param name");
            literal->value = std::move(lexer[getIndexGuard(lexer,true)].str_token);
            expression->list.emplace_back(std::move(literal));
            if (lexer[getIndexGuard(lexer)].token == T_COMMA)
            {
                (void)getIndexGuard(lexer,true);

                continue;
            }
            else
                break;
        } while (true);
        (void)getIndexGuard(lexer,true);
    }
    else
        (void)getIndexGuard(lexer,true);
    return std::move(expression);
}

ASTBlock *Parser::parseBlock(const std::vector<lexToken> &lexer) noexcept
{
    ASTBlock *block = new ASTBlock();
    if (lexer[getIndexGuard(lexer,true)].token != T_R_SQUIGGLY)
    {
        do
        {
            if (lexer.size() <= getIndexGuard(lexer))
            {
                error(lexer[lexer.size() - 1], "there was no was to know when a block ended");
                break;
            }
            if (lexer[getIndexGuard(lexer)].token == T_RET)
            {
                (void)getIndexGuard(lexer, true);
                ASTExpression *node = new ASTExpression();
                node->type = AST_EXPR_RETURN;
                uint32_t fake_i = getIndexGuard(lexer);
                fake_i++;
                if (lexer[fake_i].token == T_DOT)
                    fake_i += 2;

                if (isBinaryExpr(lexer[fake_i]))
                {
                    const ASTBinaryExpression *expr = parseBinaryExpr(0, lexer);
                    [[unlikely]] if (!expr)
                        expr = new ASTBinaryExpression();
                    node->list.emplace_back(std::move(expr));
                }
                else if (!isInteger(lexer[getIndexGuard(lexer)]) && !isValidName(lexer[getIndexGuard(lexer)]))
                {
                    ASTExpression *expr = new ASTExpression();
                    expr->type = AST_EXPR_NO_RETURN;
                    node->list.emplace_back(std::move(expr));
                }
                else if (isValidFuncCall(lexer))
                {
                    puts("ret: function call hit");
                    std::exit(0);
                }
                else
                {
                    ASTLiteral *literal = parseLiteral(lexer);
                    [[unlikely]] if (!literal)
                        literal = new ASTLiteral();
                    if (lexer[getIndexGuard(lexer) - 2].token == T_DOT)
                        literal->data_type = FLOAT_ANY_TYPE;
                    else if (lexer[getIndexGuard(lexer) - 1].token == T_IDENTIFIER && !isInteger(lexer[getIndexGuard(lexer) - 1]))
                        literal->data_type = NOT_DETERMINED_DATA_TYPE;
                    else
                        literal->data_type = UINT_ANY_TYPE;
                    node->list.emplace_back(std::move(literal));
                }
                block->list.emplace_back(std::move(node));
            }
            else if (isValidFuncCall(lexer))
            {
                puts("func global: function call hit");
                std::exit(0);
            }
            else if (isDataType(lexer[getIndexGuard(lexer)]))
                block->list.emplace_back(parseVar(lexer));
            else
                error(lexer[getIndexGuard(lexer,true)], "this is not a valid in this scope");

        } while (lexer[getIndexGuard(lexer)].token != T_R_SQUIGGLY);
    }
    else
        (void)getIndexGuard(lexer,true);
    return block;
}

void Parser::parse(const std::string &&source) noexcept
{
    std::vector<lexToken> lexer;
    {
        uint32_t line = 0;
        auto words_begin = std::sregex_token_iterator(source.begin(), source.end(), regex);
        auto words_end = std::sregex_token_iterator();
        std::vector<std::string> split;
        split.reserve(std::distance(words_begin, words_end));
        std::move(words_begin, words_end, std::back_inserter(split));
        lexer.reserve(split.size());
        uint32_t i = 1;
        const uint32_t size = split.size() - 1;
        bool skip = false;
        for (const auto &word : split)
        {
            if (skip)
            {
                skip = false;
                const uint32_t n = ++i;
                i = std::min<uint32_t>(size, n);
                continue;
            }
            auto o = reject.find(std::hash<std::string>{}(word));
            if (o != NULL)
            {
                if (*o)
                    line++;
                const uint32_t n = ++i;
                i = std::min<uint32_t>(size, n);
                continue;
            }
            lexToken token;
            auto t = lookup.find(std::hash<std::string>{}(word));
            if (t == NULL)
            {
                token.token = T_IDENTIFIER;
                token.str_token = std::move(word);
            }
            else
            {
                if (i < split.size())
                {
                    auto k = lookup.find(std::hash<std::string>{}(split[i]));
                    if (k == NULL)
                        token.token = *t;
                    else
                    {
                        if ((*k) == T_L_ARROW && (*t) == T_SUB)
                        {
                            token.token = T_S_ARROW;
                            skip = true;
                        }
                        else
                            token.token = *t;
                    }
                }
            }
            token.line = line;
            const uint32_t n = ++i;
            i = std::min<uint32_t>(size, n);
            if (lexer.size() < i)
                lexer.emplace_back(token);
        }
        lexer.shrink_to_fit();
    }
    // end lexer here
    //------------------------------------------------------------------------------------
    // now we parse
    for (; lexer_index < lexer.size(); lexer_index++)
    {
        if (lexer[getIndexGuard(lexer)].token == T_FUNC || lexer[getIndexGuard(lexer)].token == T_PUBLIC_FUNC)
        {
            bool public_ = lexer[getIndexGuard(lexer,true)].token != T_FUNC;
            [[unlikely]] if (!isValidName(lexer[getIndexGuard(lexer)]))
                error(lexer[getIndexGuard(lexer)], "this is not a valid function name");
            ASTFuncDef *node = new ASTFuncDef(std::move(lexer[getIndexGuard(lexer,true)].str_token), public_);
            if (lexer[getIndexGuard(lexer)].token != T_L_CURLY)
                error(lexer[getIndexGuard(lexer,true)], "this is not a valid function args opener");
            node->args = parseArgs(lexer);
            if (lexer[getIndexGuard(lexer)].token != T_S_ARROW)
            {
                if (isDataType(lexer[getIndexGuard(lexer)]))
                    error(lexer[getIndexGuard(lexer,true)],
                          "you forgot to declare the return type -> to tell me what you are returning");
                else if (lexer[getIndexGuard(lexer,true)].token != T_L_SQUIGGLY)
                    error(lexer[getIndexGuard(lexer,true)], "syntax error garbage return type");
                else
                    node->return_type = VOID_TYPE;
            }
            else
            {
                if (!isDataType(lexer[getIndexGuard(lexer, true)]))
                    error(lexer[getIndexGuard(lexer,true)], "this is not a supported data type");
                node->return_type = parseDataType(lexer[getIndexGuard(lexer,true)]);
            }
            if (lexer[getIndexGuard(lexer)].token != T_L_SQUIGGLY)
                error(lexer[getIndexGuard(lexer)], "this is not a valid function block opener");
            else
                node->body = parseBlock(lexer);
            root->children.emplace_back(std::move(node));

            if (node->return_type == VOID_TYPE &&
                (node->body->list.empty() || node->body->list.back()->type != AST_EXPR_RETURN))
            {
                ASTExpression *node_ = new ASTExpression();
                node_->type = AST_EXPR_RETURN;
                ASTExpression *expr = new ASTExpression();
                expr->type = AST_EXPR_NO_RETURN;
                node_->list.emplace_back(std::move(expr));
                node->body->list.emplace_back(node_);
            }
        }
        else if (isDataType(lexer[getIndexGuard(lexer)]))
            root->children.emplace_back(parseVar(lexer,true));
        else
        {
            error(lexer[getIndexGuard(lexer)], "this is not a valid in global scope");
        }
    }
}
} // namespace LunaScript::compiler::front