#include "Parser.h"
namespace LunaScript::compiler::front
{
static const std::ReadOnlyLookupTable<data_size_t, Parser::LexToken> lookup = {
    {std::hash<std::string>{}("void"), Parser::LexToken::NO_RETURN},
    {std::hash<std::string>{}("int8"), Parser::LexToken::INT8},
    {std::hash<std::string>{}("int16"), Parser::LexToken::INT16},
    {std::hash<std::string>{}("int32"), Parser::LexToken::INT32},
    {std::hash<std::string>{}("int64"), Parser::LexToken::INT64},
    {std::hash<std::string>{}("uint8"), Parser::LexToken::UINT8},
    {std::hash<std::string>{}("uint16"), Parser::LexToken::UINT16},
    {std::hash<std::string>{}("uint32"), Parser::LexToken::UINT32},
    {std::hash<std::string>{}("uint64"), Parser::LexToken::UINT64},
    {std::hash<std::string>{}("float32"), Parser::LexToken::FLOAT32},
    {std::hash<std::string>{}("float64"), Parser::LexToken::FLOAT64},
    {std::hash<std::string>{}("func"), Parser::LexToken::FUNC},
    {std::hash<std::string>{}("public"), Parser::LexToken::PUBLIC_FUNC},
    {std::hash<std::string>{}("ret"), Parser::LexToken::RET},
    {std::hash<std::string>{}("{"), Parser::LexToken::L_SQUIGGLY},
    {std::hash<std::string>{}("}"), Parser::LexToken::R_SQUIGGLY},
    {std::hash<std::string>{}("("), Parser::LexToken::L_CURLY},
    {std::hash<std::string>{}(")"), Parser::LexToken::R_CURLY},
    {std::hash<std::string>{}(","), Parser::LexToken::COMMA},
    {std::hash<std::string>{}("."), Parser::LexToken::DOT},
    {std::hash<std::string>{}("+"), Parser::LexToken::ADD},
    {std::hash<std::string>{}("-"), Parser::LexToken::SUB},
    {std::hash<std::string>{}("/"), Parser::LexToken::DIV},
    {std::hash<std::string>{}("*"), Parser::LexToken::MUL},
    {std::hash<std::string>{}("="), Parser::LexToken::EQUAL},
    {std::hash<std::string>{}("and"), Parser::LexToken::AND},
    {std::hash<std::string>{}("or"), Parser::LexToken::OR},
    {std::hash<std::string>{}(">"), Parser::LexToken::L_ARROW},
    {std::hash<std::string>{}("<"), Parser::LexToken::R_ARROW},
    {std::hash<std::string>{}("mod"), Parser::LexToken::MODULO},
    {std::hash<std::string>{}("\""), Parser::LexToken::QUOTE},
    {std::hash<std::string>{}("\'"), Parser::LexToken::SINGLE_LETTER_QUOTE},
    {std::hash<std::string>{}("xor"), Parser::LexToken::XOR},
};

static const std::ReadOnlyLookupTable<data_size_t, bool> reject = {
    {std::hash<std::string>{}("\n"), true},
    {std::hash<std::string>{}("\r"), false},
    {std::hash<std::string>{}(" "), false},
    {std::hash<std::string>{}("\t"), false},
};
static const std::regex regex(
    "(([A-Za-z0-9_-]+)|(\\s)|([\\\",\\\\',\\(,\\),\\[,\\],\\<,\\>,\\{,\\},\\,:,\\.])|([+,=,*,/]+))",
    std::regex_constants::icase);

ASTLiteral *Parser::parseLiteral(const std::vector<lexToken> &lexer) noexcept
{
    ASTLiteral *literal = new ASTLiteral();
    if (lexer[getIndexGuard(lexer)].token != LexToken::QUOTE)
    {
        [[unlikely]] if (!isInteger(lexer[getIndexGuard(lexer)]) && !isValidName(lexer[getIndexGuard(lexer)]))
            error(lexer[getIndexGuard(lexer)], "this is not a valid numeric value or variable name");
        literal->value = std::move(lexer[getIndexGuard(lexer, true)].str_token);
        if (lexer[getIndexGuard(lexer)].token == LexToken::DOT)
        {
            [[unlikely]] if (!isInteger(lexer[getIndexGuard(lexer, true)]) && !isValidName(lexer[getIndexGuard(lexer)]))
                error(lexer[getIndexGuard(lexer)], "this is not a valid numeric value or variable name");
            literal->value += '.';
            literal->value += std::move(lexer[getIndexGuard(lexer, true)].str_token);
        }
        else if (literal->data_type == DataType::FLOAT32 || literal->data_type == DataType::FLOAT64)
        {
            literal->value += ".0";
        }
    }
    else
        error(lexer[getIndexGuard(lexer)], "we don't have string support yet");
    return std::move(literal);
}

const ASTBinaryExpression *Parser::parseBinaryExpr(const uint8_t precedence_inflator,
                                                   const std::vector<lexToken> &lexer) noexcept
{
    ASTBinaryExpression *expr = new ASTBinaryExpression();
    ASTLiteral *lit_1 = parseLiteral(lexer);
    if (lexer[getIndexGuard(lexer) - 2].token == LexToken::DOT)
        lit_1->data_type = DataType::ANY_FLOAT;
    else if (lexer[getIndexGuard(lexer) - 1].token == LexToken::IDENTIFIER &&
             !isInteger(lexer[getIndexGuard(lexer) - 1]))
        lit_1->data_type = DataType::NOT_DETERMINED;
    else
        lit_1->data_type = DataType::ANY_UINT;
    expr->left = std::move(lit_1);
    const std::tuple<uint16_t, OperatorType> type = parseOPType(lexer[getIndexGuard(lexer, true)]);
    expr->op = std::get<1>(type);
    expr->precedence = std::get<0>(type) + precedence_inflator;
    uint32_t fake_i = std::min<uint32_t>(getIndexGuard(lexer) + 1, lexer.size());
    if (lexer[fake_i].token == LexToken::DOT)
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
        if (lexer[getIndexGuard(lexer) - 2].token == LexToken::DOT)
            lit_2->data_type = DataType::ANY_FLOAT;
        else if (lexer[getIndexGuard(lexer) - 1].token == LexToken::IDENTIFIER &&
                 !isInteger(lexer[getIndexGuard(lexer) - 1]))
            lit_2->data_type = DataType::NOT_DETERMINED;
        else
            lit_2->data_type = DataType::ANY_UINT;
        expr->right = std::move(lit_2);
    }
    return expr;
}

const ASTExpression *Parser::parseVar(const std::vector<lexToken> &lexer, bool is_global) noexcept
{
    ASTExpression *node = new ASTExpression();
    node->type = ExpressionType::VAR_DEFINED;
    [[unlikely]] if (!isDataType(lexer[getIndexGuard(lexer)]))
    {
        error(lexer[getIndexGuard(lexer)], "this is not a supported data type");
        return node;
    }
    DataType data_type = parseDataType(lexer[getIndexGuard(lexer, true)]);
    node->data_type = data_type;
    [[unlikely]] if (!isValidName(lexer[getIndexGuard(lexer)]))
        error(lexer[getIndexGuard(lexer)], "this is not a valid param name");
    node->extra_data = std::move(lexer[getIndexGuard(lexer, true)].str_token);
    if (lexer[getIndexGuard(lexer)].token != LexToken::EQUAL)
    {
        if (isInteger(lexer[getIndexGuard(lexer)]))
            error(lexer[getIndexGuard(lexer)], "you missing the equals(=) to define value of the variable");
        ASTLiteral *literal = new ASTLiteral();
        literal->data_type = data_type;
        if (literal->data_type >= DataType::INT8 && literal->data_type <= DataType::UINT64)
            literal->value = "0";
        else if (literal->data_type >= DataType::FLOAT32 && literal->data_type <= DataType::FLOAT64)
            literal->value = "0.0";
        else
            literal->value = "";
        node->list.emplace_back(std::move(literal));
    }
    else
    {
        (void)getIndexGuard(lexer, true);

        if (isValidFuncCall(lexer) && !is_global)
        {
            [[unlikely]] if (!isValidName(lexer[getIndexGuard(lexer)]))
                error(lexer[getIndexGuard(lexer)], "this is not a valid function name");
            std::string name = lexer[getIndexGuard(lexer, true)].str_token;
            const ASTFuncCall *func_call = new ASTFuncCall(name, std::move(parseArgs(lexer)));
            node->list.emplace_back(func_call);
        }
        if (!isInteger(lexer[getIndexGuard(lexer)]) && !isValidName(lexer[getIndexGuard(lexer)]))
        {
            error(lexer[getIndexGuard(lexer)], "no good stating data for variable provided");
            return node;
        }
        uint32_t fake_i = getIndexGuard(lexer);
        fake_i++;
        if (lexer[fake_i].token == LexToken::DOT)
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
    expression->type = ExpressionType::PRAM_LIST;
    if (lexer[getIndexGuard(lexer)].token == LexToken::L_CURLY)
        (void)getIndexGuard(lexer, true);
    if (lexer[getIndexGuard(lexer)].token != LexToken::R_CURLY)
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
            literal->data_type = parseDataType(lexer[getIndexGuard(lexer, true)]);
            [[unlikely]] if (!isValidName(lexer[getIndexGuard(lexer)]))
                error(lexer[getIndexGuard(lexer)], "this is not a valid param name");
            literal->value = std::move(lexer[getIndexGuard(lexer, true)].str_token);
            expression->list.emplace_back(std::move(literal));
            if (lexer[getIndexGuard(lexer)].token == LexToken::COMMA)
            {
                (void)getIndexGuard(lexer, true);

                continue;
            }
            else
                break;
        } while (true);
        (void)getIndexGuard(lexer, true);
    }
    else
        (void)getIndexGuard(lexer, true);
    return std::move(expression);
}

ASTBlock *Parser::parseBlock(const std::vector<lexToken> &lexer) noexcept
{
    ASTBlock *block = new ASTBlock();
    if (lexer[getIndexGuard(lexer, true)].token != LexToken::R_SQUIGGLY)
    {
        if (lexer[getIndexGuard(lexer)].token == LexToken::R_SQUIGGLY)
        {
            (void)getIndexGuard(lexer, true);
            return block;
        }
        do
        {
            if (lexer.size() <= getIndexGuard(lexer))
            {
                error(lexer[getIndexGuard(lexer)], "there was no was to know when a block ended");
                break;
            }
            if (lexer[getIndexGuard(lexer)].token == LexToken::RET)
            {
                (void)getIndexGuard(lexer, true);
                ASTExpression *node = new ASTExpression();
                node->type = ExpressionType::RETURN;
                uint32_t fake_i = getIndexGuard(lexer);
                fake_i++;
                if (lexer[fake_i].token == LexToken::DOT)
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
                    expr->type = ExpressionType::NO_RETURN;
                    node->list.emplace_back(std::move(expr));
                }
                else if (isValidFuncCall(lexer))
                {
                    [[unlikely]] if (!isValidName(lexer[getIndexGuard(lexer)]))
                        error(lexer[getIndexGuard(lexer)], "this is not a valid function name");
                    std::string name = lexer[getIndexGuard(lexer, true)].str_token;
                    const ASTFuncCall *func_call = new ASTFuncCall(name, std::move(parseArgs(lexer)));
                    node->list.emplace_back(func_call);
                }
                else
                {
                    ASTLiteral *literal = parseLiteral(lexer);
                    [[unlikely]] if (!literal)
                        literal = new ASTLiteral();
                    if (lexer[getIndexGuard(lexer) - 2].token == LexToken::DOT)
                        literal->data_type = DataType::ANY_FLOAT;
                    else if (lexer[getIndexGuard(lexer) - 1].token == LexToken::IDENTIFIER &&
                             !isInteger(lexer[getIndexGuard(lexer) - 1]))
                        literal->data_type = DataType::NOT_DETERMINED;
                    else
                        literal->data_type = DataType::ANY_UINT;
                    node->list.emplace_back(std::move(literal));
                }
                block->list.emplace_back(std::move(node));
            }
            else if (isValidFuncCall(lexer))
            {
                [[unlikely]] if (!isValidName(lexer[getIndexGuard(lexer)]))
                    error(lexer[getIndexGuard(lexer)], "this is not a valid function name");
                std::string name = lexer[getIndexGuard(lexer, true)].str_token;
                const ASTFuncCall *func_call = new ASTFuncCall(name, std::move(parseArgs(lexer)));
                block->list.emplace_back(func_call);
            }
            else if (isDataType(lexer[getIndexGuard(lexer)]))
                block->list.emplace_back(parseVar(lexer));
            else
                error(lexer[getIndexGuard(lexer, true)], "this is not a valid in this scope");

        } while (lexer[getIndexGuard(lexer)].token != LexToken::R_SQUIGGLY);
    }
    else
        (void)getIndexGuard(lexer, true);
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
                token.token = LexToken::IDENTIFIER;
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
                        if ((*k) == LexToken::L_ARROW && (*t) == LexToken::SUB)
                        {
                            token.token = LexToken::S_ARROW;
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
        if (lexer[getIndexGuard(lexer)].token == LexToken::FUNC ||
            lexer[getIndexGuard(lexer)].token == LexToken::PUBLIC_FUNC)
        {
            bool public_ = lexer[getIndexGuard(lexer, true)].token != LexToken::FUNC;
            [[unlikely]] if (!isValidName(lexer[getIndexGuard(lexer)]))
                error(lexer[getIndexGuard(lexer)], "this is not a valid function name");
            ASTFuncDef *node = new ASTFuncDef(std::move(lexer[getIndexGuard(lexer, true)].str_token), public_);
            if (lexer[getIndexGuard(lexer)].token != LexToken::L_CURLY)
                error(lexer[getIndexGuard(lexer, true)], "this is not a valid function args opener");
            node->args = parseArgs(lexer);
            if (lexer[getIndexGuard(lexer)].token != LexToken::S_ARROW)
            {
                if (isDataType(lexer[getIndexGuard(lexer)]))
                    error(lexer[getIndexGuard(lexer, true)],
                          "you forgot to declare the return type -> to tell me what you are returning");
                else if (lexer[getIndexGuard(lexer)].token != LexToken::L_SQUIGGLY)
                    error(lexer[getIndexGuard(lexer, true)], "syntax error garbage return type");
                else
                    node->return_type = DataType::VOID;
            }
            else
            {
                (void)getIndexGuard(lexer, true);
                if (!isDataType(lexer[getIndexGuard(lexer)]))
                    error(lexer[getIndexGuard(lexer, true)], "this is not a supported data type");
                node->return_type = parseDataType(lexer[getIndexGuard(lexer, true)]);
            }
            if (lexer[getIndexGuard(lexer)].token != LexToken::L_SQUIGGLY)
            {
                error(lexer[getIndexGuard(lexer)], "this is not a valid function block opener");
                break;
            }
            else
                node->body = parseBlock(lexer);
            root->children.emplace_back(std::move(node));

            if (node->return_type == DataType::VOID &&
                (node->body->list.empty() || node->body->list.back()->type != ExpressionType::RETURN))
            {
                ASTExpression *node_ = new ASTExpression();
                node_->type = ExpressionType::RETURN;
                ASTExpression *expr = new ASTExpression();
                expr->type = ExpressionType::NO_RETURN;
                node_->list.emplace_back(std::move(expr));
                node->body->list.emplace_back(node_);
            }
            else if (node->return_type != DataType::VOID &&
                     (node->body->list.empty() || node->body->list.back()->type != ExpressionType::RETURN))
                error(lexer[getIndexGuard(lexer)], "a data expected return type must explicitly return a value");
        }
        else if (isDataType(lexer[getIndexGuard(lexer)]))
            root->children.emplace_back(parseVar(lexer, true));
        else
        {
            error(lexer[getIndexGuard(lexer)], "this is not a valid in global scope");
        }
    }
}
} // namespace LunaScript::compiler::front