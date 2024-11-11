#include <stdio.h>
#include <assert.h>
#include "../include/front_end.hpp"
#include "../include/print_in_log.hpp"
#include "../include/file_processing.hpp"
#include "../include/lexer.hpp"

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
        case FRONT_ERROR_STK:
            return "Frontend: Ошибка работы стэка.";
        case FRONT_ERROR_PROC_FILE:
            return "Frontend: Ошибка обработки файла.";
        case FRONT_ERROR_CALLOC:
            return "Frontend: Ошибка выделения памяти.";
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
    StkError      stk_error       = STK_ERROR_OK;
    ProcFileError proc_file_error = PROC_FILE_NO_ERROR;

    front->output_file = fopen (name_of_output_file, "wb");
    if (front->output_file == NULL)
    {
        front_error = FRONT_ERROR_FOPEN;
        goto close_input_file;
    }

    stk_error = stack_ctor (&(front->vars), sizeof (Vars));
    if (stk_error)
    {
        stk_print_error (stk_error);
        front_error = FRONT_ERROR_STK;
        goto close_all_files;
    }

    proc_file_error = read_file_count_size (name_of_input_file, front->input_file, &(front->size_of_file), &(front->input_buffer));
    if (proc_file_error)
    {
        proc_file_print_error (proc_file_error);
        front_error = FRONT_ERROR_PROC_FILE;
        goto stack_destructor;
    }

    front->tokens = (LexInfo*) calloc (front->size_of_file, sizeof (LexInfo));
    if (front->tokens == NULL)
    {
        front_error = FRONT_ERROR_CALLOC;
        goto free_memory;
    }

    goto out;

free_memory:
    free (front->input_buffer);
stack_destructor:
    stack_dtor (&(front->vars));
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
    LexError   lex_error = fill_tokens (front->tokens, front->input_buffer);
}

FrontError frontend_dtor (FrontInfo* front)
{

}
