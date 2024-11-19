#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include "../include/front_end.hpp"
#include "../include/print_in_log.hpp"
#include "../include/file_processing.hpp"
#include "../include/lexer.hpp"
#include "../include/parser.hpp"

const size_t INITIAL_CAPACITY = 25;

static FILE* log_file = stderr;

void frontend_set_log_file (FILE* file)
{
    log_file = file;
}

void frontend_print_error (FrontError error)
{
    PRINT ("%s\n", frontend_get_error (error));
}

const char* frontend_get_error (FrontError error)
{
    switch (error)
    {
        case FRONT_ERROR_OK:
            return "Frontend: Ошибок не обнаружено.";
        case FRONT_ERROR_SETVBUF:
            return "Frontend: Ошибка отключения буферизации.";
        case FRONT_ERROR_NULL_PTR_SRUCT:
            return "Frontend: Передан нулевой указатель на структуру.";
        case FRONT_ERROR_FOPEN:
            return "Frontend: Ошибка открытия файла.";
        case FRONT_ERROR_DYN_ARR:
            return "Frontend: Ошибка работы динамического массива.";
        case FRONT_ERROR_PROC_FILE:
            return "Frontend: Ошибка обработки файла.";
        case FRONT_ERROR_CALLOC:
            return "Frontend: Ошибка выделения памяти.";
        case FRONT_ERROR_LEX:
            return "Frontend: Ошибка лексера.";
        case FRONT_ERROR_PARSE:
            return "Frontend: Ошибка парсера.";
        default:
            return "Frontend: Нужной ошибки не найдено...";
    }
}

FrontError frontend_ctor (FrontInfo* front, const char* name_of_input_file, const char* name_of_output_file)
{
    assert (front);

    if (front == NULL)
        return FRONT_ERROR_NULL_PTR_SRUCT;

    front->input_file = fopen (name_of_input_file, "r");
    if (front->input_file == NULL)
        return FRONT_ERROR_FOPEN;

    FrontError    front_error     = FRONT_ERROR_OK;
    DynArrError   dyn_arr_error   = ARR_ERROR_OK;
    ProcFileError proc_file_error = PROC_FILE_NO_ERROR;

    front->output_file = fopen (name_of_output_file, "wb");
    if (front->output_file == NULL)
    {
        front_error = FRONT_ERROR_FOPEN;
        goto close_input_file;
    }

    dyn_arr_error = dyn_array_ctor (&(front->tokens), INITIAL_CAPACITY, sizeof (LexInfo));
    if (dyn_arr_error)
    {
        dyn_array_print_error (dyn_arr_error);
        front_error = FRONT_ERROR_DYN_ARR;
        goto close_all_files;
    }

    dyn_arr_error = dyn_array_ctor (&(front->vars), INITIAL_CAPACITY, sizeof (Var));
    if (dyn_arr_error)
    {
        dyn_array_print_error (dyn_arr_error);
        front_error = FRONT_ERROR_DYN_ARR;
        goto dyn_array_dtor;
    }

    proc_file_error = read_file_count_size (name_of_input_file, front->input_file, &(front->size_of_file), &(front->input_buffer));
    if (proc_file_error)
    {
        proc_file_print_error (proc_file_error);
        front_error = FRONT_ERROR_PROC_FILE;
        goto dtor_all_arrays;
    }

    goto out;

dtor_all_arrays:
    dyn_array_dtor (&(front->vars));
dyn_array_dtor:
    dyn_array_dtor (&(front->tokens));
close_all_files:
    fclose (front->output_file);
close_input_file:
    fclose (front->input_file);
out:
    return front_error;
}

FrontError frontend_pass (FrontInfo* front)
{
    FrontError front_error = FRONT_ERROR_OK;
    LexError   lex_error = get_token (&(front->tokens), &(front->vars), front->input_buffer);
    if (lex_error)
    {
        lex_print_error (lex_error);
        return FRONT_ERROR_LEX;
    }

    ParseError parse_error = PARSE_ERROR_OK;
    Node* syntax_tree = parse (&(front->tokens), &parse_error);
    if (parse_error)
    {
        parse_print_error (parse_error);
        return FRONT_ERROR_PARSE;
    }

    return front_error;
}

// FrontError frontend_dtor (FrontInfo* front)
// {
//
// }
