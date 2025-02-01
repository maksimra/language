#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include "core/back_end.hpp"
#include "io/print_in_log.hpp"
#include "file_processing.hpp"
#include "lexer.hpp"

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
    ProcFileError proc_file_error = PROC_FILE_NO_ERROR;
    Node* root_syntax_tree = NULL;

    back->output_file = fopen(name_of_output_file, "wb");
    if (back->output_file == NULL)
    {
        back_error = BACK_ERROR_FOPEN;
        goto close_input_file;
    }

    proc_file_error = read_file_count_size(name_of_input_file,
                                           back->input_file,
                                           &(back->size_of_file),
                                           &(back->input_buffer));
    if (proc_file_error)
    {
        proc_file_print_error(proc_file_error);
        back_error = BACK_ERROR_PROC_FILE;
        goto close_all_files;
    }

    root_syntax_tree = create_tree(back->input_buffer,
                                   back->input_buffer + back->size_of_file,
                                   &back_error);

    if (back_error)
        goto delete_tree;
    back->root = root_syntax_tree;

    goto out;

delete_tree:
    tree_dtor(root_syntax_tree);
close_all_files:
    fclose(back->output_file);
close_input_file:
    fclose(back->input_file);
out:
    return back_error;
}

Node* create_tree(const char* buffer, const char* buf_end, BackError* error)
{
    assert (buffer);
    assert (buf_end);

    const int necessary_received_number = 2;

    if (*buffer != '#')
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

    TreeError tree_error = TREE_ERROR_OK;
    if (buffer < buf_end && *buffer == '_')
    {
        buffer++;
        if (buffer < buf_end && *buffer == '_')
        {
            buffer++;
            Node* node = create_node((LexType) type, value, NULL, NULL, &tree_error);
            if (tree_error)
            {
                *error = BACK_ERROR_TREE;
                return NULL;
            }

            return node;
        }
        Node* node =  create_node((LexType) type, value,
                                  NULL,
                                  create_tree(buffer, buf_end, error), &tree_error);
        if (tree_error)
        {
            *error = BACK_ERROR_TREE;
            return NULL;
        }

        return node;
    }

    Node* node = create_node((LexType) type, value,
                             create_tree(buffer, buf_end, error),
                             create_tree(buffer, buf_end, error), &tree_error);
    if (tree_error)
    {
        *error = BACK_ERROR_TREE;
        return NULL;
    } // TODO: макросом
    return node;
}

BackError backend_pass (BackInfo* back)
{
    BackError error = create_asm_file(back->output_file, back->root, NULL);
    return BACK_ERROR_OK;
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

            fprintf(output_file, "push %lg\n", node->elem.elem.num);
            break;
        case LEX_TYPE_VAR:
            if (parent->elem.elem.oper == LEX_OPER_ASSIGN &&
                node == parent->left)
                fprintf(output_file, "push %zu\n", node->elem.elem.var_number);
            else
                fprintf(output_file, "push [%zu]\n", node->elem.elem.var_number);

            break;
        case LEX_TYPE_OPER:
            OPERS[(int) node->elem.elem.oper].gen_asm(output_file);
        case LEX_TYPE_DELIM:
            break; // TODO: может кринж, может вообще не работает
        case LEX_TYPE_TXT:
        default:
            return BACK_ERROR_NODE_TYPE;
    }

    return error;
}

void backend_dtor(BackInfo *back)
{
    fclose (back->input_file);
    fclose (back->output_file);
    free   (back->input_buffer);
    tree_dtor (back->root);
}
