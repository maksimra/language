#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "../include/tokenizer.hpp"
#include "../include/skip_space.hpp"
#include "../include/void_stack.hpp"

TokenError token (TokenInfo* tok, Vars* vars, const char* buffer, int max_n_vars)
{
    TokenError error = TOK_ERROR_OK;
    int n_tok = 0;
    int n_vars = 0;
    while (*buffer != '$')
    {
        skip_space (&buffer);
        if (try_char_operation (tok, n_tok, &buffer) ||
            try_digit          (tok, n_tok, &buffer) ||
            try_parenthesis    (tok, n_tok, &buffer) ||
            try_function       (tok, n_tok, &buffer) ||
            try_var            (tok, n_tok, vars, &n_vars, max_n_vars, &buffer, &error))
        {
            n_tok++;
        }
        else
        {
            return TOK_ERROR_SYNTAX;
        }
    }

    tok[n_tok].type = TOK_TYPE_TXT;
    tok[n_tok].elem.symbol = '$';
    token_dump (tok, n_tok, vars);
    return TOK_ERROR_OK;
}

bool try_var (TokenInfo* tok, int n_tok, Vars* vars, int* n_vars, int max_n_vars, const char** buffer, DifError* error)
{
    if (isalpha (**buffer))
    {
        const char* start_position = *buffer;

        (*buffer)++;
        while (isalpha (**buffer))
            (*buffer)++;

        tok[n_tok].type = TOK_TYPE_VAR;
        int var_number = search_var (vars, *n_vars, start_position, (size_t) (*buffer - start_position));
        if (var_number != NOT_VAR)
        {
            tok[n_tok].elem.var_number = var_number;
        }
        else
        {
            if (*n_vars >= max_n_vars)
            {
                *error = DIF_ERROR_N_VARS;
                return false;
            }

            vars[*n_vars].num = *n_vars;
            vars[*n_vars].len = (size_t) (*buffer - start_position);
            vars[*n_vars].name = start_position;
            tok[n_tok].elem.var_number = *n_vars;
            (*n_vars)++;
        }
        return true;
    }
    return false;
}

bool try_function (TokenInfo* tok, int n_tok, const char** buffer)
{
    size_t len_of_function = 0;

    if (isalpha (**buffer))
    {
        len_of_function++;
        while (isalpha ((*buffer)[len_of_function]))
            len_of_function++;

        TokenOperator oper = search_oper (*buffer, len_of_function);
        if (oper != OPER_NONE)
        {
            (*buffer) += len_of_function;
            fill_token_oper (tok, n_tok, oper);
            return true;
        }
    }
    return false;
}

bool try_parenthesis (TokenInfo* tok, int n_tok, const char** buffer)
{
    if ((**buffer) == '(' || (**buffer) == ')')
    {
        tok[n_tok].type = TOK_TYPE_TXT;
        tok[n_tok].elem.symbol = **buffer;
        (*buffer)++;
        return true;
    }
    return false;
}

bool try_digit (TokenInfo* tok, int n_tok, const char** buffer)
{
    if (isdigit (**buffer))
    {
        size_t len_of_double = fill_token_double (tok, n_tok, *buffer);
        (*buffer) += len_of_double;
        return true;
    }
    return false;

}

bool try_char_operation (TokenInfo* tok, int n_tok, const char** buffer)
{
    TokenOperator oper = search_char_operation (*buffer);
    if (oper != OPER_NONE)
    {
        (*buffer)++;
        fill_token_oper (tok, n_tok, oper);
        return true;
    }
    return false;
}

void token_dump (const TokenInfo* tok, int n_tok, const Vars* vars)
{
    for (int pass = 0; pass <= n_tok; pass++)
    {
        switch (tok[pass].type)
        {
            case TOK_TYPE_OPER:
                PRINT ("OPER: %d --> %s\n", pass, get_oper_name (tok[pass].elem.oper));
                break;
            case TOK_TYPE_NUM:
                PRINT ("NUM: %d --> %lf\n", pass, tok[pass].elem.num);
                break;
            case TOK_TYPE_VAR:
                PRINT ("VAR: %d --> %.*s\n", pass, (int) vars[tok[pass].elem.var_number].len, vars[tok[pass].elem.var_number].name);
                break;
            case TOK_TYPE_TXT:
                PRINT ("TXT: %d --> %c\n", pass, tok[pass].elem.symbol);
                break;
            default:
                assert (0);
        }
    }
}

TokenOperator search_oper (const char* str, size_t len)
{
    for (int n_oper = 1; n_oper < NUM_OF_OPERS; n_oper++)
    {
        if (strncmp (str, OPER_ARRAY[n_oper].name, len) == 0)
            return OPER_ARRAY[n_oper].op_enum;
    }

    return OPER_NONE;
}

int search_var (Vars* vars, int n_vars, const char* begin, size_t len)
{
    for (int number_of_var = 0; number_of_var < n_vars; number_of_var++)
        if (strncmp (begin, vars[number_of_var].name, len) == 0)
            return number_of_var;

    return NOT_VAR;
}

void fill_token_oper (TokenInfo* tok, int n_tok, TokenOperator oper)
{
    tok[n_tok].type = TOK_TYPE_OPER;
    tok[n_tok].elem.oper = oper;
}

size_t fill_token_double (TokenInfo* tok, int n_tok, const char* buffer)
{
    double number = NAN;
    sscanf (buffer, "%lf", &number);
    tok[n_tok].type = TOK_TYPE_NUM;
    tok[n_tok].elem.num = number;

    size_t len_of_double = 0;
    bool has_point = false;
    while (isdigit(buffer[len_of_double]))
    {
        len_of_double++;
        if (buffer[len_of_double] == '.' && has_point == false)
        {
            len_of_double++;
            has_point = true;
        }
    }
    return len_of_double;
}

TokenOperator search_char_operation (const char* buffer)
{
    for (int num_of_oper = 1; OPER_ARRAY[num_of_oper].is_func == false; num_of_oper++)
    {
        if (strncmp (buffer, OPER_ARRAY[num_of_oper].name, sizeof (char)) == 0)
            return OPER_ARRAY[num_of_oper].op_enum;
    }
    return OPER_NONE;
}
