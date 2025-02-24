#include <assert.h>
#include <stdio.h>
#include "io/print_svg.hpp"
#include "io/print_in_log.hpp"
#include "dyn_array.hpp"

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

GraphError graphviz (const Node* node)
{
    assert (node != NULL);

    FILE* file = fopen("graphviz.txt", "w");
    print_start (file);

    GraphError error = print_connections (node, file);
    print_end (file);

    fclose (file);

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

GraphError print_connections (const Node* node, FILE* file)
{
    GraphError error = draw_left  (node, file);
    if (error)
        return error;

    error = draw_right (node, file);
    return error;
}

void print_end (FILE* file)
{
    fprintf (file, "}");
}

GraphError draw_right (const Node* node, FILE* file)
{
    const char* color = NULL;
    if (node->right != NULL)
    {
        if (node->right->left == NULL && node->right->right == NULL)
            color = "red";
        else
            color = "green";

        const char* name = (node->elem.type == LEX_TYPE_OPER) ?
                           OPERS[(int) node->elem.elem.oper].name :
                           DELIMS[(int) node->elem.elem.delim].name;

        switch (node->right->elem.type)
        {
            case LEX_TYPE_OPER:
                fprintf (file, "{\"%s\n%p\"--\"%s\n%p\"[color = \"%s\"]};\n",
                               name,
                               node,
                               OPERS[(int) node->right->elem.elem.oper].name,
                               node->right, color);
                break;
            case LEX_TYPE_NUM:
                fprintf (file, "{\"%s\n%p\"--\"%.3lf\n%p\"[color = \"%s\"]};\n",
                               name,
                               node,
                               node->right->elem.elem.num,
                               node->right, color);
                break;
            case LEX_TYPE_VAR:
                fprintf (file, "{\"%s\n%p\"--\"var №%zu\n%p\"[color = \"%s\"]};\n",
                               name,
                               node,
                               node->right->elem.elem.var_number,
                               node->right, color);
                break;
            case LEX_TYPE_DELIM:
                fprintf (file, "{\"%s\n%p\"--\"%s\n%p\"[color = \"%s\"]};\n",
                               name,
                               node,
                               DELIMS[(int) node->right->elem.elem.delim].name,
                               node->right, color);
                break;
            case LEX_TYPE_BRACE:
            default:
                return GRAPH_ERROR_NODE_TYPE;
        }

        print_connections (node->right, file);
    }

    return GRAPH_ERROR_OK;
}

GraphError draw_left (const Node* node, FILE* file)
{
    const char* color = NULL;
    if (node->left != NULL)
    {
        if (node->left->left == NULL && node->left->right == NULL)
            color = "red";
        else
            color = "green";

        const char* name = (node->elem.type == LEX_TYPE_OPER) ?
                           OPERS[(int) node->elem.elem.oper].name :
                           DELIMS[(int) node->elem.elem.delim].name;

        switch (node->left->elem.type)
        {
            case LEX_TYPE_OPER:
                fprintf (file, "{\"%s\n%p\"--\"%s\n%p\"[color = \"%s\"]};\n",
                               name,
                               node,
                               OPERS[(int) node->left->elem.elem.oper].name,
                               node->left, color);
                break;
            case LEX_TYPE_NUM:
                fprintf (file, "{\"%s\n%p\"--\"%.3lf\n%p\"[color = \"%s\"]};\n",
                               name,
                               node,
                               node->left->elem.elem.num,
                               node->left, color);
                break;
            case LEX_TYPE_VAR:
                fprintf (file, "{\"%s\n%p\"--\"var №%zu\n%p\"[color = \"%s\"]};\n",
                               name,
                               node,
                               node->left->elem.elem.var_number,
                               node->left, color);
                break;
            case LEX_TYPE_DELIM:
                fprintf (file, "{\"%s\n%p\"--\"%s\n%p\"[color = \"%s\"]};\n",
                               name,
                               node,
                               OPERS[(int) node->left->elem.elem.delim].name,
                               node->left, color);
                break;
            case LEX_TYPE_BRACE:
            default:
                return GRAPH_ERROR_NODE_TYPE;
        }

        print_connections (node->left, file);
    }

    return GRAPH_ERROR_OK;
}
