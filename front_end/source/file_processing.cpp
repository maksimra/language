#include <stdio.h>
#include <sys/stat.h>
#include <assert.h>
#include <stdlib.h>
#include <ctype.h>
#include "../include/file_processing.hpp"
#include "../include/print_in_log.hpp"

static FILE *log_file = stderr;

void proc_file_set_log_file(FILE *file)
{
    log_file = file;
}

ProcFileError process_file(char ***ptr_to_lines, const char *name_of_file, FILE *file, size_t *number_lines)
{
    PRINT_BEGIN();

    if (file == NULL)
        return PROC_FILE_ERROR_PTR_TO_FILE;

    ProcFileError error = PROC_FILE_NO_ERROR;
    size_t size_of_file = 0;
    char *buffer = NULL;
    error = read_file_count_size(name_of_file, file, &size_of_file, &buffer);
    if (error != PROC_FILE_NO_ERROR)
        goto free_memory;

    *number_lines = line_processing(size_of_file, buffer);
    error = make_ptr_to_lines(*number_lines, buffer, size_of_file, ptr_to_lines);

    if (error != PROC_FILE_NO_ERROR)
        goto free_memory;

    goto out;

free_memory:
    if (!buffer)
        free(buffer);
    if (!(*ptr_to_lines))
        free(*ptr_to_lines);

out:
    PRINT_END();
    return error;
}

ProcFileError read_file_count_size(const char *name_of_file, FILE *file, size_t *size, char **buffer)
{
    PRINT_BEGIN();
    struct stat statbuf = {};
    if (stat(name_of_file, &statbuf))
        return PROC_FILE_ERROR_STAT;

    *size = (size_t)statbuf.st_size;

    if (file == NULL)
        return PROC_FILE_ERROR_PTR_TO_FILE;

    *buffer = (char *)calloc(*size, sizeof(char));
    if (*buffer == NULL)
        return PROC_FILE_ERROR_CALLOC;

    size_t fread_size = fread(*buffer, sizeof(char), *size, file);
    if (fread_size != *size)
        return PROC_FILE_ERROR_FREAD;

    PRINT_END();
    return PROC_FILE_NO_ERROR;
}

size_t line_processing(size_t size, char *buffer)
{
    PRINT_BEGIN();
    size_t number_lines = 0;
    bool was_carriage_return = false;
    for (size_t k = 0; k < size; k++)
    {
        if (buffer[k] == '\r')
        {
            if (was_carriage_return)
                break;

            number_lines++;
            buffer[k] = '\0';
            k++;
            was_carriage_return = true;
            continue;
        }
        if (buffer[k] == '\n')
        {
            number_lines++;
            buffer[k] = '\0';
            continue;
        }
        was_carriage_return = false;
    }

    if (buffer[size - 1] != '\n' && buffer[size - 1] != '\0')
        number_lines++;

    PRINT_END();
    return number_lines;
}

ProcFileError make_ptr_to_lines(size_t number_lines, char *buffer, size_t buffer_size, char ***lines)
{
    PRINT_BEGIN();
    char **temp_lines = (char **)calloc(number_lines, sizeof(char *));
    if (temp_lines == NULL)
        return PROC_FILE_ERROR_CALLOC;

    *lines = temp_lines;

    size_t n_line = 0;
    (*lines)[n_line++] = buffer;
    for (size_t k = 0; n_line < number_lines && k < buffer_size; )
    {
        if (buffer[k] == '\0')
        {
            k++;
            if (k < buffer_size && buffer[k] == '\n')
                k++;

            (*lines)[n_line++] = buffer + k;
        }

        if (buffer[k] != '\0')
            k++;
    }
    PRINT_END();
    return PROC_FILE_NO_ERROR;
}

void proc_file_print_error(ProcFileError error)
{
    PRINT("%s\n", proc_file_get_error(error));
}

const char *proc_file_get_error(ProcFileError error)
{
    switch (error)
    {
        case PROC_FILE_NO_ERROR:
            return "ProcFile: Ошибок в работе функций не выявлено.";
        case PROC_FILE_ERROR_STAT:
            return "ProcFile: Ошибка вызова stat.";
        case PROC_FILE_ERROR_FOPEN:
            return "ProcFile: Ошибка открытия файла.";
        case PROC_FILE_ERROR_FREAD:
            return "ProcFile: Ошибка чтения из файла функцией fread.";
        case PROC_FILE_ERROR_CALLOC:
            return "ProcFile: Ошибка выделения памяти функцией calloc.";
        case PROC_FILE_ERROR_PTR_TO_FILE:
            return "ProcFile: Передан нулевой указатель на файл.";
        default:
            return "ProcFile: Нужной ошибки не найдено...";
    }
}
