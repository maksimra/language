#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include "../include/back_end.hpp"
#include "../include/print_in_log.hpp"
#include "../include/file_processing.hpp"
#include "../include/lexer.hpp"
#include "../include/parser.hpp"
#include "../include/print_svg.hpp"

static FILE *log_file = stderr;

void backend_set_log_file(FILE *file)
{
    log_file = file;
}

void backend_print_error(BackError error)
{
    PRINT("%s\n", backend_get_error(error));
}

const char *backend_get_error(BackError error)
{
    switch (error)
    {
    case BACK_ERROR_OK:
        return "Backend: Ошибок не обнаружено.";
    case BACK_ERROR_FOPEN:
        return "Backend: Ошибка открытия файла.";
    case BACK_ERROR_PROC_FILE:
        return "Backend: Ошибка обработки файла.";
    case BACK_ERROR_SSCANF:
        return "Backend: Ошибка работы функции sscanf.";
    case BACK_ERROR_SEMANTICS:
        return "Backend: Семантическая ошибка.";
    default:
        return "Backend: Нужной ошибки не найдено...";
    }
}

BackError backend_ctor(BackInfo* back, const char* name_of_input_file, const char* name_of_output_file)
{
    assert(back);

    back->input_file = fopen(name_of_input_file, "r");
    if (back->input_file == NULL)
        return BACK_ERROR_FOPEN;

    BackError     back_error      = BACK_ERROR_OK;
    DynArrError   dyn_arr_error   = ARR_ERROR_OK;
    ProcFileError proc_file_error = PROC_FILE_NO_ERROR;

    back->output_file = fopen(name_of_output_file, "wb");
    if (back->output_file == NULL)
    {
        back_error = BACK_ERROR_FOPEN;
        goto close_input_file;
    }

    proc_file_error = read_file_count_size(name_of_input_file, back->input_file, &(back->size_of_file), &(back->input_buffer));
    if (proc_file_error)
    {
        proc_file_print_error(proc_file_error);
        back_error = BACK_ERROR_PROC_FILE;
        goto close_all_files;
    }

    Node* root = create_tree(back->input_buffer,
                             back->input_buffer + back->size_of_file,
                             &back_error);

    if (back_error)
        goto delete_tree;
    back->root = root;

    goto out;

delete_tree:
    tree_dtor(root);
close_all_files:
    fclose(back->output_file);
close_input_file:
    fclose(back->input_file);
out:
    return back_error;
}

Node* create_tree(const char* buffer, const char* buf_end, BackError* error)
{
    assert (back);

    const int necessary_received_number = 2;

    if (buffer != '#')
    {
        *error = BACK_ERROR_READ;
        return NULL;
    }
    buffer++;

    int type = 0;
    double value = 0;

    int received_length = 0;
    int number_received = sscanf(buffer, "%d#%lf%n", &type, &value, &received_length);
    if (number_received != necessary_received_number)
    {
        *error = BACK_ERROR_SSCANF;
        return NULL;
    }

    buffer += received_length;

    if (buffer < buf_end && *buffer == '_')
    {
        buffer++;
        if (buffer < buf_end && *buffer == '_')
        {
            buffer++;
            return create_node((LexType) type, value, NULL, NULL);
        }
        return create_node((LexType) type, value,
                           NULL,
                           create_tree(buffer));
    }

    return create_node((LexType) type, value, create_tree(buffer, buf_end, error), create_tree(buffer, buf_end, error));
}

FrontError backend_pass (BackInfo* back)
{
    BackError error = create_asm_file(back->output_file, back->root, NULL);
}

BackError create_asm_file(FILE* output_file, Node* node, Node* parent)
{
    BackError error = BACK_ERROR_OK;
    if (node->right != NULL)
        error = create_asm_file(output_file, node->right, node);

    if (node->left != NULL)
        error = create_asm_file(output_file, node->left, node);

    switch (node->elem.type)
    {
        case LEX_TYPE_NUM:
            if (parent->elem.elem.oper == LEX_OPER_ASSIGN &&
                node == parent->left)
                return BACK_ERROR_SEMANTICS;

            fprintf(output_file, "push %lg\n", node->elem.num);
            break;
        case LEX_TYPE_VAR:
            if (parent->elem.elem.oper == LEX_OPER_ASSIGN &&
                node == parent->left)
                fprintf(output_file, "push %zu\n", node->elem.var_number);
            else
                fprintf(output_file, "push [%zu]\n", node->elem.var_number);

            break;
        case LEX_TYPE_OPER:
            OPERS[(int) node->elem.elem.oper].func(output_file);

    }
}

BackError backend_dtor(BackInfo *back)
{
    fclose (back->input_file);
    fclose (back->output_file);
    free   (back->input_buffer);
    free   (back->output_buffer);
    DynArrError dyn_arr_error = ARR_ERROR_OK;
    dyn_arr_error = dyn_array_dtor(&(back->tokens));
    if (dyn_arr_error)
    {
        dyn_array_print_error(dyn_arr_error);
        return BACK_ERROR_DYN_ARR;
    }

    dyn_arr_error = dyn_array_dtor(&(back->vars)); // TODO: спросить, является ли копипастом
    if (dyn_arr_error)
    {
        dyn_array_print_error(dyn_arr_error);
        return BACK_ERROR_DYN_ARR;
    } // TODO: в макрос процедуру с проверкой

    return BACK_ERROR_OK;
}
