#include "Parser.h"
#include <inttypes.h>

namespace LunaScript::parser
{
using namespace LunaScript::parser::lexer;
using namespace LunaScript::compiler::ast;

static void error(const TypedStringView &, const char *msg)
{
    fputs(msg, stderr);
    putc('\n', stderr);
    fflush(stderr);
    throw std::runtime_error("error processing not implemented");
}

static inline size_t getPos(const std::vector<TypedStringView> &lexer, const bool inc = false,
                            const bool rest = false) noexcept
{
    static size_t li;
    if (rest)
        li = 0;
    if (inc && li < (lexer.size() - 1))
        return li++;
    return li;
}

static inline void eatWhitespace(const std::vector<TypedStringView> &lexer)
{
    while (lexer[getPos(lexer)].type == TypedStringView::Type::SPACE ||
           lexer[getPos(lexer)].type == TypedStringView::Type::NEW_LINE ||
           lexer[getPos(lexer)].type == TypedStringView::Type::TAB)
    {
        if (lexer.size() - 1 == getPos(lexer))
            break;
        (void)getPos(lexer, true);
    }
}

static inline void fakeEatWhitespace(const std::vector<TypedStringView> &lexer, size_t *pos)
{
    while (lexer[*pos].type == TypedStringView::Type::SPACE || lexer[*pos].type == TypedStringView::Type::NEW_LINE ||
           lexer[*pos].type == TypedStringView::Type::TAB)
    {
        if (lexer.size() - 1 == *pos)
            break;
        *pos += 1;
    }
}

static inline bool isValidName(const TypedStringView &token) noexcept
{
    return token.type == TypedStringView::Type::IDENTIFIER;
}

static inline bool isValidFuncCall(const std::vector<TypedStringView> &lexer) noexcept
{
    return getPos(lexer) + 1 < (lexer.size() - 1)
               ? lexer[getPos(lexer)].type == TypedStringView::Type::IDENTIFIER &&
                         lexer[getPos(lexer) + 1].type == TypedStringView::Type::L_CURLY
                     ? true
                     : false
               : false;
}

static inline bool isDataType(const TypedStringView &token) noexcept
{
    switch (token.type)
    {
    case TypedStringView::Type::INT8:
    case TypedStringView::Type::UINT8:
    case TypedStringView::Type::INT16:
    case TypedStringView::Type::UINT16:
    case TypedStringView::Type::INT32:
    case TypedStringView::Type::UINT32:
    case TypedStringView::Type::INT64:
    case TypedStringView::Type::UINT64:
    case TypedStringView::Type::FLOAT32:
    case TypedStringView::Type::FLOAT64:
    case TypedStringView::Type::STRING:
        return true;
        [[unlikely]] default : return false;
    }
}

static inline bool isBinaryExpr(const TypedStringView &token) noexcept
{
    switch (token.type)
    {
    case TypedStringView::Type::ADD:
    case TypedStringView::Type::SUB:
    case TypedStringView::Type::MUL:
    case TypedStringView::Type::DIV:
    case TypedStringView::Type::AND:
    case TypedStringView::Type::OR:
    case TypedStringView::Type::XOR:
    case TypedStringView::Type::MODULO:
        return true;
    default:
        return false;
    }
}

static inline const std::tuple<uint8_t, OperatorType> parseOPType(const TypedStringView &token) noexcept
{
    switch (token.type)
    {
    case TypedStringView::Type::ADD:
        return std::make_tuple<uint8_t, OperatorType>(10, OperatorType::ADD);
    case TypedStringView::Type::SUB:
        return std::make_tuple<uint8_t, OperatorType>(20, OperatorType::SUB);
    case TypedStringView::Type::MUL:
        return std::make_tuple<uint8_t, OperatorType>(30, OperatorType::MUL);
    case TypedStringView::Type::DIV:
        return std::make_tuple<uint8_t, OperatorType>(40, OperatorType::DIV);
    case TypedStringView::Type::AND:
        return std::make_tuple<uint8_t, OperatorType>(60, OperatorType::AND);
    case TypedStringView::Type::OR:
        return std::make_tuple<uint8_t, OperatorType>(70, OperatorType::OR);
    case TypedStringView::Type::XOR:
        return std::make_tuple<uint8_t, OperatorType>(80, OperatorType::XOR);
    case TypedStringView::Type::MODULO:
        return std::make_tuple<uint8_t, OperatorType>(50, OperatorType::MOD);
        [[unlikely]] default : return std::make_tuple<uint8_t, OperatorType>(0, OperatorType::NOT_DETERMINED);
    }
}

static inline DataType parseDataType(const TypedStringView &token) noexcept
{
    switch (token.type)
    {
    case TypedStringView::Type::INT8:
        return DataType::INT8;
    case TypedStringView::Type::UINT8:
        return DataType::UINT8;
    case TypedStringView::Type::INT16:
        return DataType::INT16;
    case TypedStringView::Type::UINT16:
        return DataType::UINT16;
    case TypedStringView::Type::INT32:
        return DataType::INT32;
    case TypedStringView::Type::UINT32:
        return DataType::UINT32;
    case TypedStringView::Type::INT64:
        return DataType::INT64;
    case TypedStringView::Type::UINT64:
        return DataType::UINT64;
    case TypedStringView::Type::FLOAT32:
        return DataType::FLOAT32;
    case TypedStringView::Type::FLOAT64:
        return DataType::FLOAT64;
        [[unlikely]] default : return DataType::NOT_DETERMINED;
    }
}

static inline ASTLiteral *parseLiteral(const std::vector<TypedStringView> &lexer)
{
    ASTLiteral *literal = new ASTLiteral();
    if (lexer[getPos(lexer)].type != TypedStringView::Type::STRING)
    {
        if (lexer[getPos(lexer)].type == TypedStringView::Type::SUB)
        {
            (void)getPos(lexer, true);
            if (literal->data_type == DataType::NOT_DETERMINED)
                literal->data_type = DataType::ANY_INT;
        }
        [[unlikely]] if (lexer[getPos(lexer)].type != TypedStringView::Type::NUMBER_IDENTIFIER &&
                         !isValidName(lexer[getPos(lexer)]))
            error(lexer[getPos(lexer)], "this is not a valid numeric value or variable name");
        literal->value = std::wstring(lexer[getPos(lexer, true)].span);
        if (!isValidName(lexer[getPos(lexer)]))
        {
            if (lexer[getPos(lexer)].type == TypedStringView::Type::DOT)
            {
                (void)getPos(lexer, true);
                [[unlikely]] if (lexer[getPos(lexer)].type != TypedStringView::Type::NUMBER_IDENTIFIER)
                    error(lexer[getPos(lexer)], "this is not a valid numeric value or variable name");
                literal->value += L'.';
                literal->value += lexer[getPos(lexer, true)].span;
                if (literal->data_type == DataType::NOT_DETERMINED)
                    literal->data_type = DataType::ANY_FLOAT;
            }
            else if (literal->data_type == DataType::FLOAT32 || literal->data_type == DataType::FLOAT64)
                literal->value += L".0";
            else
            {
                if (literal->data_type == DataType::NOT_DETERMINED)
                    literal->data_type = DataType::ANY_UINT;
            }
        }
    }
    else
        error(lexer[getPos(lexer)], "we don't have string support yet");

    eatWhitespace(lexer);
    return literal;
}

static inline ASTParamListExpression *parseArgs(const std::vector<TypedStringView> &lexer, bool call_mode)
{
    std::vector<const ASTNode *> prams;
    if (lexer[getPos(lexer)].type == TypedStringView::Type::L_CURLY)
    {
        (void)getPos(lexer, true);
        eatWhitespace(lexer);
    }

    if (lexer[getPos(lexer)].type != TypedStringView::Type::R_CURLY)
    {
        size_t n = 0;
        if (!call_mode)
        {
            do
            {
                [[unlikely]] if (n >= 100)
                {
                    error(lexer[getPos(lexer)], "too many arguments or bad syntax");
                    break;
                }
                [[unlikely]] if (!isDataType(lexer[getPos(lexer)]))
                    error(lexer[getPos(lexer)], "this is not a supported data type");
                ASTLiteral *literal = new ASTLiteral();
                literal->data_type = parseDataType(lexer[getPos(lexer, true)]);
                eatWhitespace(lexer);
                [[unlikely]] if (!isValidName(lexer[getPos(lexer)]))
                    error(lexer[getPos(lexer)], "this is not a valid param name");
                literal->value = std::wstring(lexer[getPos(lexer, true)].span);
                eatWhitespace(lexer);
                prams.emplace_back(std::move(literal));
                if (lexer[getPos(lexer)].type == TypedStringView::Type::COMMA)
                {
                    (void)getPos(lexer, true);
                    eatWhitespace(lexer);
                    n++;
                    continue;
                }
                else
                    break;
            } while (true);
            (void)getPos(lexer, true);
            eatWhitespace(lexer);
        }
        else
        {
            do
            {
                [[unlikely]] if (n >= 100)
                {
                    error(lexer[getPos(lexer)], "too many arguments or bad syntax");
                    break;
                }
                prams.emplace_back(std::move(parseLiteral(lexer)));
                if (lexer[getPos(lexer)].type == TypedStringView::Type::COMMA)
                {
                    (void)getPos(lexer, true);
                    eatWhitespace(lexer);
                    n++;
                    continue;
                }
                else
                    break;
            } while (true);
        }
        if (lexer[getPos(lexer)].type == TypedStringView::Type::R_CURLY)
        {
            (void)getPos(lexer, true);
            eatWhitespace(lexer);
        }
    }
    else
    {
        (void)getPos(lexer, true);
        eatWhitespace(lexer);
    }
    return new ASTParamListExpression(std::ReadOnlyVector<const ASTNode *>::lock(prams));
}

static inline const ASTBinaryExpression *parseBinaryExpr(const uint8_t precedence_inflator,
                                                         const std::vector<TypedStringView> &lexer)
{
    ASTBinaryExpression *expr = new ASTBinaryExpression();
    expr->left = parseLiteral(lexer);
    const std::tuple<uint16_t, OperatorType> type = parseOPType(lexer[getPos(lexer, true)]);
    eatWhitespace(lexer);
    expr->op = std::get<1>(type);
    expr->precedence = std::get<0>(type) + precedence_inflator;
    size_t fake_i = getPos(lexer) + 1;
    fakeEatWhitespace(lexer, &fake_i);
    if (lexer[fake_i].type == TypedStringView::Type::DOT)
        fake_i += 3;
    fakeEatWhitespace(lexer, &fake_i);
    if (isBinaryExpr(lexer[std::min((size_t)lexer.size() - 1, fake_i)]))
    {
        const ASTBinaryExpression *r_expr = parseBinaryExpr(1 + precedence_inflator, lexer);
        [[unlikely]] if (!r_expr) r_expr = new ASTBinaryExpression();
        expr->right = std::move(r_expr);
    }
    else
    {
        if (lexer[getPos(lexer)].type != TypedStringView::Type::NUMBER_IDENTIFIER && !isValidName(lexer[getPos(lexer)]))
        {
            error(lexer[getPos(lexer)], "no good data or variable provided");
            return expr;
        }
        expr->right = parseLiteral(lexer);
    }
    return expr;
}

static inline const ASTVarDef *parseVar(const std::vector<TypedStringView> &lexer, bool is_global)
{
    ASTVarDef *node = new ASTVarDef();
    node->global = is_global;
    [[unlikely]] if (!isDataType(lexer[getPos(lexer)]))
    {
        error(lexer[getPos(lexer)], "this is not a supported data type");
        return node;
    }
    DataType data_type = parseDataType(lexer[getPos(lexer, true)]);
    eatWhitespace(lexer);
    node->data_type = data_type;
    [[unlikely]] if (!isValidName(lexer[getPos(lexer)])) error(lexer[getPos(lexer)], "this is not a valid param name");
    node->name = std::wstring(lexer[getPos(lexer, true)].span);
    eatWhitespace(lexer);
    if (lexer[getPos(lexer)].type != TypedStringView::Type::EQUAL)
    {
        if (lexer[getPos(lexer)].type == TypedStringView::Type::NUMBER_IDENTIFIER)
            error(lexer[getPos(lexer)], "you missing the equals(=) to define value of the variable");
        node->data_type = data_type;
        if (node->data_type >= DataType::INT8 && node->data_type <= DataType::UINT64)
            node->value = L"0";
        else if (node->data_type >= DataType::FLOAT32 && node->data_type <= DataType::FLOAT64)
            node->value = L"0.0";
        else
            node->value = L"";
    }
    else
    {
        (void)getPos(lexer, true);
        eatWhitespace(lexer);
        if (!is_global && isValidFuncCall(lexer))
        {
            [[unlikely]] if (!isValidName(lexer[getPos(lexer)]))
                error(lexer[getPos(lexer)], "this is not a valid function name");
            std::wstring name = std::wstring(lexer[getPos(lexer, true)].span);
            eatWhitespace(lexer);
            node->child = new ASTFuncCall(name, std::move(parseArgs(lexer, true)));
        }
        else if (lexer[getPos(lexer)].type != TypedStringView::Type::NUMBER_IDENTIFIER &&
                 !isValidName(lexer[getPos(lexer)]))
        {
            error(lexer[getPos(lexer)], "no good stating data for variable provided");
            return node;
        }
        else
        {
            eatWhitespace(lexer);
            size_t fake_i = getPos(lexer) + 1;
            if (lexer[fake_i].type == TypedStringView::Type::DOT)
                fake_i += 2;
            fakeEatWhitespace(lexer, &fake_i);
            if (isBinaryExpr(lexer[fake_i]))
                node->child = parseBinaryExpr(0, lexer);
            else
            {
                const ASTLiteral *literal = parseLiteral(lexer);
                node->value = literal->value;
                delete literal;
            }
        }
    }
    return std::move(node);
}

static inline ASTBlock *parseBlock(const std::vector<TypedStringView> &lexer)
{
    ASTBlock *block = new ASTBlock();
    if (lexer[getPos(lexer, true)].type != TypedStringView::Type::R_SQUIGGLY)
    {
        eatWhitespace(lexer);
        if (lexer[getPos(lexer)].type == TypedStringView::Type::R_SQUIGGLY)
        {
            (void)getPos(lexer, true);
            eatWhitespace(lexer);
            return block;
        }
        do
        {
            if ((lexer.size() - 1) <= getPos(lexer))
            {
                error(lexer[getPos(lexer)], "there was no was to know when a block ended");
                break;
            }
            if (lexer[getPos(lexer)].type == TypedStringView::Type::RET)
            {
                (void)getPos(lexer, true);
                eatWhitespace(lexer);
                ASTReturnExpression *node = new ASTReturnExpression();
                size_t fake_i = getPos(lexer) + 1;
                fakeEatWhitespace(lexer, &fake_i);
                if (lexer[std::min((size_t)lexer.size() - 1, fake_i + 1)].type == TypedStringView::Type::DOT)
                    fake_i += 3;
                fakeEatWhitespace(lexer, &fake_i);
                if (isBinaryExpr(lexer[std::min(lexer.size() - 1, fake_i)]))
                {
                    const ASTBinaryExpression *expr = parseBinaryExpr(0, lexer);
                    [[unlikely]] if (!expr) expr = new ASTBinaryExpression();
                    node->child = expr;
                }
                else if (lexer[getPos(lexer)].type != TypedStringView::Type::NUMBER_IDENTIFIER &&
                         !isValidName(lexer[getPos(lexer)]))
                    node->child = new ASTNoReturnExpression();
                else if (isValidFuncCall(lexer))
                {
                    [[unlikely]] if (!isValidName(lexer[getPos(lexer)]))
                        error(lexer[getPos(lexer)], "this is not a valid function name");
                    std::wstring name = std::wstring(lexer[getPos(lexer, true)].span);
                    eatWhitespace(lexer);
                    node->child = new ASTFuncCall(name, std::move(parseArgs(lexer, true)));
                }
                else
                    node->child = parseLiteral(lexer);
                block->list.emplace_back(std::move(node));
            }
            else if (isValidFuncCall(lexer))
            {
                [[unlikely]] if (!isValidName(lexer[getPos(lexer)]))
                    error(lexer[getPos(lexer)], "this is not a valid function name");
                std::wstring name = std::wstring(lexer[getPos(lexer, true)].span);
                eatWhitespace(lexer);
                const ASTFuncCall *func_call = new ASTFuncCall(name, std::move(parseArgs(lexer, true)));
                block->list.emplace_back(func_call);
            }
            else if (isDataType(lexer[getPos(lexer)]))
                block->list.emplace_back(parseVar(lexer, false));
            else
                error(lexer[getPos(lexer, true)], "this is not a valid in this scope");

        } while (lexer[getPos(lexer)].type != TypedStringView::Type::R_SQUIGGLY);
        if (lexer[getPos(lexer)].type == TypedStringView::Type::R_SQUIGGLY)
        {
            (void)getPos(lexer, true);
            eatWhitespace(lexer);
        }
    }
    else
    {
        (void)getPos(lexer, true);
        eatWhitespace(lexer);
    }
    return block;
}

static inline ASTModule *parseModule(const std::vector<TypedStringView> &lexer)
{
    (void)getPos(lexer, false, true);
    if (lexer[getPos(lexer)].type != TypedStringView::Type::MODULE)
        error(lexer[getPos(lexer)], "Invalid module");
    (void)getPos(lexer, true);
    eatWhitespace(lexer);

    [[unlikely]] if (!isValidName(lexer[getPos(lexer)])) error(lexer[getPos(lexer)], "this is not a valid module name");

    ASTModule *module_ = new ASTModule(std::wstring(lexer[getPos(lexer, true)].span));
    eatWhitespace(lexer);
    for (; getPos(lexer) < (lexer.size() - 1);)
    {
        if (lexer[getPos(lexer)].type == TypedStringView::Type::FUNC ||
            lexer[getPos(lexer)].type == TypedStringView::Type::PUBLIC_FUNC)
        {
            bool public_ = lexer[getPos(lexer, true)].type != TypedStringView::Type::FUNC;
            eatWhitespace(lexer);
            [[unlikely]] if (!isValidName(lexer[getPos(lexer)]))
                error(lexer[getPos(lexer)], "this is not a valid function name");
            ASTFuncDef *node = new ASTFuncDef(std::wstring(lexer[getPos(lexer, true)].span), public_);
            eatWhitespace(lexer);
            if (lexer[getPos(lexer)].type != TypedStringView::Type::L_CURLY)
                error(lexer[getPos(lexer, true)], "this is not a valid function args opener");
            node->args = parseArgs(lexer, false);
            if (lexer[getPos(lexer)].type != TypedStringView::Type::S_ARROW)
            {
                if (isDataType(lexer[getPos(lexer)]))
                    error(lexer[getPos(lexer, true)],
                          "you forgot to declare the return type -> to tell me what you are returning");
                else if (lexer[getPos(lexer)].type != TypedStringView::Type::L_SQUIGGLY)
                    error(lexer[getPos(lexer, true)], "syntax error garbage return type");
                else
                    node->return_type = DataType::VOID;
            }
            else
            {
                (void)getPos(lexer, true);
                eatWhitespace(lexer);
                if (!isDataType(lexer[getPos(lexer)]))
                    error(lexer[getPos(lexer, true)], "this is not a supported data type");
                node->return_type = parseDataType(lexer[getPos(lexer, true)]);
                eatWhitespace(lexer);
            }
            if (lexer[getPos(lexer)].type != TypedStringView::Type::L_SQUIGGLY)
            {
                error(lexer[getPos(lexer)], "this is not a valid function block opener");
                break;
            }
            else
                node->body = parseBlock(lexer);
            module_->children.emplace_back(std::move(node));

            if (node->return_type == DataType::VOID &&
                (node->body->list.empty() || node->body->list.back()->getType() != NodeType::RETURN))
                node->body->list.emplace_back(new ASTNoReturnExpression());
            else if (node->return_type != DataType::VOID &&
                     (node->body->list.empty() || node->body->list.back()->getType() != NodeType::RETURN))
                error(lexer[getPos(lexer)], "a data expected return type must explicitly return a value");
        }
        else if (isDataType(lexer[getPos(lexer)]))
            module_->children.emplace_back(parseVar(lexer, false));
        else if (lexer[getPos(lexer)].type == TypedStringView::Type::GLOBAL)
        {
            (void)getPos(lexer, true);
            eatWhitespace(lexer);
            if (!isDataType(lexer[getPos(lexer)]))
                error(lexer[getPos(lexer)],
                      "not a data type that can be accepted in the current scope or is not a valid data type");
            else
                module_->children.emplace_back(parseVar(lexer, true));
        }
        else
            error(lexer[getPos(lexer)], "this is not a valid in global scope");
    }
    (void)getPos(lexer, false, true);
    return module_;
}

ASTRoot *Parser::operator()(const std::vector<TypedStringView> &lexer) const
{
    (void)getPos(lexer, false, true);
    ASTRoot *root = new ASTRoot();
    root->modules.push_back(parseModule(lexer));
    (void)getPos(lexer, false, true);
    return root;
}
} // namespace LunaScript::parser