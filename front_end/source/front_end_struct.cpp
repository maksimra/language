#include "../include/front_end_struct.hpp"
#include "../include/print_in_log.hpp"
#include "../include/file_processing.hpp"
#include <stdio.h>

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
        default:
            return "Frontend: Нужной ошибки не найдено...";
    }
}

FrontError frontend_ctor (Frontend* str, const char* name_of_input_file, const char* name_of_output_file)
{
    assert (str);
    if (str == NULL)
        return FRONT_ERROR_NULL_PTR_SRUCT;

    FILE* temp_file = fopen (name_of_input_file, "r");
    if (temp_file == NULL)
        return FRONT_ERROR_FOPEN;
    str->input_file == temp_file;

    FrontError    front_error     = FRONT_ERROR_OK;
    StkError      stk_error       = STK_ERROR_OK;
    ProcFileError proc_file_error = PROC_FILE_NO_ERROR;
    temp_file = fopen (name_of_output_file, "wb");
    if (temp_file == NULL)
    {
        front_error = FRONT_ERROR_FOPEN;
        goto close_input_file;
    }
    str->output_file = temp_file;

    stk_error = stack_ctor (&(str->vars), sizeof (Vars));
    if (stk_error)
    {
        stk_print_error (stk_error);
        front_error = FRONT_ERROR_STK;
        goto close_all_files;
    }

    proc_file_error = 

close_all_files:
    fclose (str->output_file);
close_input_file:
    fclose (str->input_file);
}

FrontError  frontend_pass         (Frontend* str)
{

}

FrontError  frontend_dtor         (Frontend* str)
{

}
