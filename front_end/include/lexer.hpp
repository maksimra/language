#ifndef LEXER_HPP
#define LEXER_HPP

enum LexError
{
    LEX_ERROR_OK      = 0,
    LEX_ERROR_SYNTAX  = 1,
    LEX_ERROR_DYN_ARR = 2,
    LEX_ERROR_STRTOD  = 3
};

enum LexType
{
    LEX_TYPE_OPER = 1,
    LEX_TYPE_NUM  = 2,
    LEX_TYPE_VAR  = 3,
    LEX_TYPE_TXT  = 4
};

enum LexOperator
{
    LEX_OPER_NONE   = 0,
    LEX_OPER_ADD    = 1,
    LEX_OPER_SUB    = 2,
    LEX_OPER_MUL    = 3,
    LEX_OPER_DIV    = 4,
    LEX_OPER_POW    = 5,
    LEX_OPER_ASSIGN = 6,
    LEX_OPER_SIN    = 7,
    LEX_OPER_COS    = 8,
    LEX_OPER_LN     = 9,
    LEX_OPER_SQRT   = 10,
    LEX_OPER_EXP    = 11
};

struct LexOpers
{
    LexOperator op_enum;
    const char* name;
    size_t len;
    bool is_func;
};

const LexOpers OPERS[] =
{
    {LEX_OPER_NONE},
    {LEX_OPER_ADD,    "+",    1, false},
    {LEX_OPER_SUB,    "-",    1, false},
    {LEX_OPER_MUL,    "*",    1, false},
    {LEX_OPER_DIV,    "/",    1, false},
    {LEX_OPER_POW,    "^",    1, false},
    {LEX_OPER_ASSIGN, "=",    1, false},
    {LEX_OPER_SIN,    "sin",  3, true},
    {LEX_OPER_COS,    "cos",  3, true},
    {LEX_OPER_LN,     "ln",   2, true},
    {LEX_OPER_SQRT,   "sqrt", 4, true},
    {LEX_OPER_EXP,    "exp",  3, true}
};

const NUM_OPERS = sizeof (OPERS) / sizeof (OPERS[0]);

union LexElem
{
    double num;
    char symbol;
    LexOperator oper;
    int var_number;
};

struct LexInfo
{
    LexType type;
    union LexElem elem;
};

struct Var
{
    const char* name;
    size_t len;
};

LexError fill_tokens (LexInfo* tokens, char* input_buffer);

#endif // LEXER_HPP
