#include <assert.h>
#include <stdio.h>
#include "../include/print_svg.hpp"
#include "../include/parser.hpp"
#include "../include/print_in_log.hpp"
#include "../include/dyn_array.hpp"

#define LEFT_VAR_NAME  ((Var*) vars->data + node->left->elem.elem.var_number)->name
#define LEFT_VAR_LEN   (int)(((Var*) vars->data + node->left->elem.elem.var_number)->len)
#define RIGHT_VAR_NAME ((Var*) vars->data + node->right->elem.elem.var_number)->name
#define RIGHT_VAR_LEN  (int)(((Var*) vars->data + node->right->elem.elem.var_number)->len)

static FILE* log_file = stderr;

void graph_set_log_file (FILE* file)
{
    log_file = file;
}

void graph_print_error (GraphError error)
{
    PRINT ("%s\n", graph_get_error (error));
}

const char* graph_get_error (GraphError error)
{
    switch (error)
    {
        case GRAPH_ERROR_OK:
            return "Graph: Ошибок не обнаружено.";
        case GRAPH_ERROR_NODE_TYPE:
            return "Graph: Ошибка узлового типа.";
        default:
            return "Graph: Нужной ошибки не найдено...";
    }
}

GraphError graphviz (const Node* node, const Darray* vars, FILE* file)
{
    assert (node != NULL);
    assert (file != NULL);

    print_start (file);

    GraphError error = print_connections (node, vars, file);
    print_end (file);

    if (error)
        return error;

    const char* cmd = "dot graphviz.txt -Tsvg -otree.svg";
    system (cmd);
    return GRAPH_ERROR_OK;
}

void print_start (FILE* file)
{
    fprintf (file, "graph G {\n");
}

GraphError print_connections (const Node* node, const Darray* vars, FILE* file)
{
    GraphError error = draw_left  (node, vars, file);
    if (error)
        return error;

    error = draw_right (node, vars, file);
    return error;
}

void print_end (FILE* file)
{
    fprintf (file, "}");
}

GraphError draw_right (const Node* node, const Darray* vars, FILE* file)
{
    const char* color = NULL;
    if (node->right != NULL)
    {
        if (node->right->left == NULL && node->right->right == NULL)
            color = "red";
        else
            color = "green";

        switch (node->right->elem.type)
        {
            case LEX_TYPE_OPER:
                fprintf (file, "{\"%s\n%p\"--\"%s\n%p\"[color = \"%s\"]};\n",
                               OPERS[(int) node->elem.elem.oper].name ,
                               node,
                               OPERS[(int) node->right->elem.elem.oper].name,
                               node->right, color);
                break;
            case LEX_TYPE_NUM:
                fprintf (file, "{\"%s\n%p\"--\"%.3lf\n%p\"[color = \"%s\"]};\n",
                               OPERS[(int) node->elem.elem.oper].name,
                               node,
                               node->right->elem.elem.num,
                               node->right, color);
                break;
            case LEX_TYPE_VAR:
                fprintf (file, "{\"%s\n%p\"--\"%.*s\n%p\"[color = \"%s\"]};\n",
                               OPERS[(int) node->elem.elem.oper].name,
                               node,
                               RIGHT_VAR_LEN,
                               RIGHT_VAR_NAME,
                               node->right, color);
                break;
            case LEX_TYPE_TXT:
            default:
                return GRAPH_ERROR_NODE_TYPE;
        }

        print_connections (node->right, vars, file);
    }

    return GRAPH_ERROR_OK;
}

GraphError draw_left (const Node* node, const Darray* vars, FILE* file)
{
    const char* color = NULL;
    if (node->left != NULL)
    {
        if (node->left->left == NULL && node->left->right == NULL)
            color = "red";
        else
            color = "green";

        switch (node->left->elem.type)
        {
            case LEX_TYPE_OPER:
            {
                fprintf (file, "{\"%s\n%p\"--\"%s\n%p\"[color = \"%s\"]};\n",
                               OPERS[(int) node->elem.elem.oper].name,
                               node,
                               OPERS[(int) node->left->elem.elem.oper].name,
                               node->left, color);
            }
                break;
            case LEX_TYPE_NUM:
                fprintf (file, "{\"%s\n%p\"--\"%.3lf\n%p\"[color = \"%s\"]};\n",
                               OPERS[(int) node->elem.elem.oper].name,
                               node,
                               node->left->elem.elem.num,
                               node->left, color);
                break;
            case LEX_TYPE_VAR:
                fprintf (file, "{\"%s\n%p\"--\"%.*s\n%p\"[color = \"%s\"]};\n",
                               OPERS[(int) node->elem.elem.oper].name,
                               node,
                               LEFT_VAR_LEN,
                               LEFT_VAR_NAME,
                               node->left, color);
                break;
            case LEX_TYPE_TXT:
            default:
                return GRAPH_ERROR_NODE_TYPE;
        }

        print_connections (node->left, vars, file);
    }

    return GRAPH_ERROR_OK;
}
