#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <stdlib.h>
#include <cstdint>

#include "lexer.hpp"
#include "utils/skip_space.hpp"
#include "dyn_array.hpp"
#include "io/print_in_log.hpp"

#define VAR_NAME ((Var*) vars->data + var_number)->name

static FILE* log_file = stderr;

LexError get_token (Darray* tokens, Darray* vars, char* input_buffer) // TODO: не забыть навесить константы на указатели-параметры
{
    assert (tokens);

    LexError error = LEX_ERROR_OK;
    skip_space (&input_buffer);

    while (*input_buffer != '\0')
    {
        if (try_char_operation (tokens, &(input_buffer), &error) ||
            try_delim          (tokens, &(input_buffer), &error) ||
            try_digit          (tokens, &(input_buffer), &error) ||
            try_parenthesis    (tokens, &(input_buffer), &error) ||
            try_keyword        (tokens, &(input_buffer), &error) ||
            try_function       (tokens, &(input_buffer), &error) ||
            try_var            (tokens, vars, &(input_buffer), &error))
        {
            skip_space (&input_buffer);
            continue;
        }
        else
        {
            return LEX_ERROR_SYNTAX;
        }
    }
    token_dump (tokens, vars);
    return LEX_ERROR_OK;
}

bool try_var (Darray* tokens, Darray* vars, char** buffer, LexError* error)
{
    if (isalpha (**buffer))
    {
        char* start_position = *buffer;

        (*buffer)++;
        while (isalpha (**buffer))
            (*buffer)++;

        size_t var_len = (size_t) (*buffer - start_position);
        size_t var_number = search_var (vars, start_position, var_len);
        LexElem elem = {};
        if (var_number != SIZE_MAX)
        {
            elem = {.var_number = var_number};
        }
        else
        {
            elem = {.var_number = vars->size};
            *error = fill_new_var (vars, start_position, var_len);
        }

        LexInfo token = {LEX_TYPE_VAR, elem};

        DynArrError dyn_arr_error = dyn_array_push (tokens, &token);
        if (dyn_arr_error)
        {
            dyn_array_print_error (dyn_arr_error);
            *error = LEX_ERROR_DYN_ARR;
        }
        return true;
    }
    return false;
}

bool try_keyword (Darray* tokens, char** buffer, LexError* error)
{
    if (isalpha (**buffer)) // TODO: спросить, ничего ли, что много одинаковых функций, который просто работают с разными данными
    {
        size_t len_of_keyword = 1;
        while (isalpha ((*buffer)[len_of_keyword]))
            len_of_keyword++;

        LexKeyword keyword = search_keyword (*buffer, len_of_keyword);
        if (keyword != LEX_KEYWORD_NONE)
        {
            (*buffer) += len_of_keyword;
            *error = fill_token_keyword (tokens, keyword);
            return true;
        }
    }
    return false;
}

bool try_function (Darray* tokens, char** buffer, LexError* error)
{
    if (isalpha (**buffer))
    {
        size_t len_of_function = 1;
        while (isalpha ((*buffer)[len_of_function]))
            len_of_function++;

        LexOperator oper = search_oper (*buffer, len_of_function);
        if (oper != LEX_OPER_NONE)
        {
            (*buffer) += len_of_function;
            *error = fill_token_oper (tokens, oper);
            return true;
        }
    }
    return false;
}

bool try_parenthesis (Darray* tokens, char** buffer, LexError* error)
{
    char next_symbol = **buffer;
    if (next_symbol == '(' || next_symbol == ')')
    {
        LexElem elem = {.brace = next_symbol};
        LexInfo token = {LEX_TYPE_BRACE, elem};

        DynArrError stk_error = dyn_array_push (tokens, &token);
        if (stk_error)
        {
            dyn_array_print_error (stk_error);
            *error = LEX_ERROR_DYN_ARR;
        }
        (*buffer)++;
        return true;
    }
    return false;
}

bool try_digit (Darray* tokens, char** buffer, LexError* error)
{
    if (isdigit (**buffer))
    {
        *error = fill_token_double (tokens, buffer);
        return true;
    }
    return false;

}

bool try_char_operation (Darray* tokens, char** buffer, LexError* error)
{
    LexOperator oper = search_char_operation (*buffer);
    if (oper != LEX_OPER_NONE)
    {
        (*buffer)++;
        *error = fill_token_oper (tokens, oper);
        return true;
    }
    return false;
}

bool try_delim (Darray* tokens, char** buffer, LexError* error)
{
    LexDelim delim = search_delim (*buffer);
    if (delim != LEX_DEL_NONE)
    {
        (*buffer)++;
        *error = fill_token_delim (tokens, delim);
        return true;
    }
    return false;
}

void token_dump (const Darray* tokens, const Darray* vars) // TODO: исправить кринж darray
{
    for (size_t pass = 0; pass < tokens->size; pass++)
    {
        switch (((LexInfo*) tokens->data + pass)->type)
        {
            case LEX_TYPE_OPER:
                printf ("OPER: %zu --> %s\n",  pass, OPERS[(int) ((LexInfo*) tokens->data + pass)->elem.oper].name); // TODO: обёртку
                break;
            case LEX_TYPE_NUM:
                printf ("NUM: %zu --> %lf\n",  pass, ((LexInfo*) tokens->data + pass)->elem.num);
                break;
            case LEX_TYPE_VAR:
                printf ("VAR: %zu --> %.*s\n", pass, (int) ((Var*) vars->data + (((LexInfo*) tokens->data + pass)->elem.var_number))->len, ((Var*) vars->data + (((LexInfo*) tokens->data + pass)->elem.var_number))->name);
                break;
            case LEX_TYPE_BRACE:
                printf ("TXT: %zu --> %c\n",   pass, ((LexInfo*) tokens->data + pass)->elem.brace);
                break;
            case LEX_TYPE_DELIM:
                printf ("DELIM: %zu --> %s\n", pass, DELIMS[(int) ((LexInfo*) tokens->data + pass)->elem.delim].name);
                break;
            default:
                assert (0);
        }
    }
}

LexKeyword search_keyword (char* str, size_t len)
{
    for (int n_keyword = 1; n_keyword < NUM_KEYWORDS; n_keyword++)
    {
        if (strncmp (str, KEYWORDS[n_keyword].name, len) == 0)
            return KEYWORDS[n_keyword].keyword_enum;
    }

    return LEX_KEYWORD_NONE;
}

LexOperator search_oper (char* str, size_t len)
{
    for (int n_oper = 1; n_oper < NUM_OPERS; n_oper++)
    {
        if (strncmp (str, OPERS[n_oper].name, len) == 0)
            return OPERS[n_oper].op_enum;
    }

    return LEX_OPER_NONE;
}

size_t search_var (Darray* vars, char* begin, size_t len)
{
    for (size_t var_number = 0; var_number < vars->size; var_number++)
        if (strncmp (begin, VAR_NAME, len) == 0)
            return var_number;

    return SIZE_MAX;
}

LexError fill_new_var (Darray* vars, char* name, size_t len)
{
    Var new_var = {.name = name, .len = len};

    DynArrError dyn_arr_error = ARR_ERROR_OK;
    dyn_arr_error = dyn_array_push (vars, &new_var);

    if (dyn_arr_error)
    {
        dyn_array_print_error (dyn_arr_error);
        return LEX_ERROR_DYN_ARR;
    }

    return LEX_ERROR_OK;
}

LexError fill_token_keyword (Darray* tokens, LexKeyword keyword)
{
    LexElem elem  = {.keyword = keyword};
    LexInfo token = {LEX_TYPE_KEYWORD, elem};

    DynArrError error = dyn_array_push (tokens, &token);
    if (error)
    {
        dyn_array_print_error (error);
        return LEX_ERROR_DYN_ARR;
    }

    return LEX_ERROR_OK;
}

LexError fill_token_oper (Darray* tokens, LexOperator oper)
{
    LexElem elem  = {.oper = oper};
    LexInfo token = {LEX_TYPE_OPER, elem};

    DynArrError error = dyn_array_push (tokens, &token);
    if (error)
    {
        dyn_array_print_error (error);
        return LEX_ERROR_DYN_ARR;
    }

    return LEX_ERROR_OK;
}

LexError fill_token_delim (Darray* tokens, LexDelim delim)
{
    LexElem elem = {.delim = delim};
    LexInfo token = {LEX_TYPE_DELIM, elem};

    DynArrError error = dyn_array_push (tokens, &token);
    if (error)
    {
        dyn_array_print_error (error);
        return LEX_ERROR_DYN_ARR;
    }

    return LEX_ERROR_OK;
}

LexError fill_token_double (Darray* tokens, char** buffer)
{
    char* old_pointer = *buffer;
    double number = strtod (*buffer, buffer);
    if (old_pointer == *buffer || errno == ERANGE)
        return LEX_ERROR_STRTOD;

    LexElem elem = {.num = number};
    LexInfo token = {LEX_TYPE_NUM, elem};

    DynArrError stk_error = dyn_array_push (tokens, &token);
    if (stk_error)
    {
        dyn_array_print_error (stk_error);
        return LEX_ERROR_DYN_ARR;
    }

    return LEX_ERROR_OK;
}

LexOperator search_char_operation (char* buffer)
{
    for (int num_of_oper = 1; num_of_oper < NUM_OPERS; num_of_oper++)
    {
        if (OPERS[num_of_oper].len == 1 &&
            strncmp (buffer, OPERS[num_of_oper].name, sizeof (char)) == 0)
        {
            return OPERS[num_of_oper].op_enum;
        }
    }
    return LEX_OPER_NONE;
}

LexDelim search_delim (char* buffer)
{
    for (int num_of_del = 1; num_of_del < NUM_DELS; num_of_del++)
    {
        if (strncmp (buffer, DELIMS[num_of_del].name, sizeof (char)) == 0)
            return DELIMS[num_of_del].delim_enum;
    }
    return LEX_DEL_NONE;
}

const char* lex_get_error (LexError error)
{
    switch (error)
    {
        case LEX_ERROR_OK:
            return "Lex: Ошибок в работе функций не выявлено.";
        case LEX_ERROR_SYNTAX:
            return "Lex: Синтаксическая ошибка.";
        case LEX_ERROR_DYN_ARR:
            return "Lex: Ошибка динамического массива.";
        case LEX_ERROR_STRTOD:
            return "Lex: Ошибка с strtod.";
        default:
            return "Lex: Нужной ошибки не найдено...";
    }
}

void lex_print_error (LexError error)
{
    PRINT ("%s\n", lex_get_error (error));
}
