#ifndef LEXER_HPP
#define LEXER_HPP

#include "dyn_array.hpp"

enum LexError
{
    LEX_ERROR_OK      = 0,
    LEX_ERROR_SYNTAX  = 1,
    LEX_ERROR_DYN_ARR = 2,
    LEX_ERROR_STRTOD  = 3
};

enum LexType
{
    LEX_TYPE_OPER    = 0,
    LEX_TYPE_NUM     = 1,
    LEX_TYPE_VAR     = 2,
    LEX_TYPE_DELIM   = 3,
    LEX_TYPE_KEYWORD = 4,
    LEX_TYPE_BRACE   = 5
};

enum LexOperator
{
    LEX_OPER_NONE      = 0,
    LEX_OPER_ADD       = 1,
    LEX_OPER_SUB       = 2,
    LEX_OPER_MUL       = 3,
    LEX_OPER_DIV       = 4,
    LEX_OPER_POW       = 5,
    LEX_OPER_ASSIGN    = 6,
    LEX_OPER_GREATER   = 7,
    LEX_OPER_EQGREATER = 8,
    LEX_OPER_LESS      = 9,
    LEX_OPER_EQLESS    = 10,
    LEX_OPER_EQUAL     = 11,
    LEX_OPER_SIN       = 12,
    LEX_OPER_COS       = 13,
    LEX_OPER_LN        = 14,
    LEX_OPER_SQRT      = 15,
    LEX_OPER_EXP       = 16
};


struct LexOpers
{
    LexOperator op_enum;
    const char* name;
    size_t len;
    bool is_func;

    #ifdef BACKEND
    void (*gen_asm) (FILE* output_file);
    #endif
};

#ifdef BACKEND
void asm_add    (FILE* output_file);
void asm_sub    (FILE* output_file);
void asm_mul    (FILE* output_file);
void asm_div    (FILE* output_file);
void asm_pow    (FILE* output_file);
void asm_assign (FILE* output_file);
void asm_sin    (FILE* output_file);
void asm_cos    (FILE* output_file);
void asm_ln     (FILE* output_file);
void asm_sqrt   (FILE* output_file);
void asm_exp    (FILE* output_file);
#endif

const LexOpers OPERS[] =
{
    #ifdef BACKEND
    {LEX_OPER_NONE},
    {LEX_OPER_ADD,       "+",    1, false, asm_add},
    {LEX_OPER_SUB,       "-",    1, false, asm_sub},
    {LEX_OPER_MUL,       "*",    1, false, asm_mul},
    {LEX_OPER_DIV,       "/",    1, false, asm_div},
    {LEX_OPER_POW,       "^",    1, false, asm_pow},
    {LEX_OPER_ASSIGN,    "=",    1, false, asm_assign},
    {LEX_OPER_SIN,       "sin",  3, true,  asm_sin},
    {LEX_OPER_COS,       "cos",  3, true,  asm_cos},
    {LEX_OPER_LN,        "ln",   2, true,  asm_ln},
    {LEX_OPER_SQRT,      "sqrt", 4, true,  asm_sqrt},
    {LEX_OPER_EXP,       "exp",  3, true,  asm_exp}
    #else
    {LEX_OPER_NONE},
    {LEX_OPER_ADD,       "+",    1, false},
    {LEX_OPER_SUB,       "-",    1, false},
    {LEX_OPER_MUL,       "*",    1, false},
    {LEX_OPER_DIV,       "/",    1, false},
    {LEX_OPER_POW,       "^",    1, false},
    {LEX_OPER_ASSIGN,    "=",    1, false},
    {LEX_OPER_GREATER,   ">",    1, false},
    {LEX_OPER_EQGREATER, "=>",   2, false},
    {LEX_OPER_LESS,      "<",    1, false},
    {LEX_OPER_EQLESS,    "=<",   2, false},
    {LEX_OPER_EQUAL,     "==",   2, false},
    {LEX_OPER_SIN,       "sin",  3, true},
    {LEX_OPER_COS,       "cos",  3, true},
    {LEX_OPER_LN,        "ln",   2, true},
    {LEX_OPER_SQRT,      "sqrt", 4, true},
    {LEX_OPER_EXP,       "exp",  3, true}
    #endif
};


const int NUM_OPERS = sizeof (OPERS) / sizeof (OPERS[0]);

enum LexDelim
{
    LEX_DEL_NONE        = 0,
    LEX_DEL_SEMICOLON   = 1,
    LEX_DEL_LEFT_BRACE  = 2,
    LEX_DEL_RIGHT_BRACE = 3
};

enum LexKeyword
{
    LEX_KEYWORD_NONE  = 0,
    LEX_KEYWORD_IF    = 1,
    LEX_KEYWORD_WHILE = 2
};

struct LexDels
{
    LexDelim delim_enum;
    const char* name;
};

const LexDels DELIMS[] =
{
    {LEX_DEL_NONE},
    {LEX_DEL_SEMICOLON,   ";"},
    {LEX_DEL_LEFT_BRACE,  "{"},
    {LEX_DEL_RIGHT_BRACE, "}"}
};

const int NUM_DELS = sizeof (DELIMS) / sizeof (DELIMS[0]);

union LexElem
{
    double      num;
    char        brace;
    LexOperator oper;
    LexDelim    delim;
    LexKeyword  keyword;
    size_t      var_number;
};

struct LexKeywords
{
    LexKeyword keyword_enum;
    const char* name;
};

const LexKeywords KEYWORDS[] =
{
    {LEX_KEYWORD_NONE},
    {LEX_KEYWORD_IF,    "if"},
    {LEX_KEYWORD_WHILE, "while"}
};

const int NUM_KEYWORDS = sizeof (KEYWORDS) / sizeof (KEYWORDS[0]);

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

LexError    get_token             (Darray* tokens, Darray* vars, char* input_buffer);
bool        try_char_operation    (Darray* tokens, char** buffer, LexError* error);
bool        try_delim             (Darray* tokens, char** buffer, LexError* error);
bool        try_digit             (Darray* tokens, char** buffer, LexError* error);
bool        try_parenthesis       (Darray* tokens, char** buffer, LexError* error);
bool        try_keyword           (Darray* tokens, char** buffer, LexError* error);
bool        try_function          (Darray* tokens, char** buffer, LexError* error);
bool        try_var               (Darray* tokens, Darray* vars, char** buffer, LexError* error);
LexKeyword  search_keyword        (char* str, size_t len);
LexOperator search_oper           (char* str, size_t len);
size_t      search_var            (Darray* vars, char* begin, size_t len);
LexOperator search_char_operation (char* buffer);
LexDelim    search_delim          (char* buffer);
LexError    fill_new_var          (Darray* vars, char* name, size_t len);
LexError    fill_token_oper       (Darray* tokens, LexOperator oper);
LexError    fill_token_keyword    (Darray* tokens, LexKeyword keyword);
LexError    fill_token_delim      (Darray* tokens, LexDelim delim);
LexError    fill_token_double     (Darray* tokens, char** buffer);
void        token_dump            (const Darray* tokens, const Darray* vars);
void        lex_print_error       (LexError error);
const char* lex_get_error         (LexError error);


#endif // LEXER_HPP
