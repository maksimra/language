#ifndef TOKENIZER_HPP
#define TOKENIZER_HPP

enum TokenError
{
    TOK_ERROR_OK     = 0,
    TOK_ERROR_SYNTAX = 1
};

enum TokenType
{
    TOK_TYPE_OPER = 1,
    TOK_TYPE_NUM  = 2,
    TOK_TYPE_VAR  = 3,
    TOK_TYPE_TXT  = 4
};

enum TokenOperator
{
    OPER_NONE = 0,
    OPER_ADD  = 1,
    OPER_SUB  = 2,
    OPER_MUL  = 3,
    OPER_DIV  = 4,
    OPER_POW  = 5,
    OPER_SIN  = 6,
    OPER_COS  = 7,
    OPER_LN   = 8,
    OPER_SQRT = 9,
    OPER_EXP  = 10
};

union TokenElem
{
    double num;
    char symbol;
    TokenOperator oper;
    size_t var_number;
};

struct TokenInfo
{
    TokenType type;
    union TokenElem elem;
};

struct Vars
{
    const char* name;
    size_t len;
};

#endif // TOKENIZER_HPP
