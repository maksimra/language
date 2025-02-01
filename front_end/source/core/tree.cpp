#include <stdio.h>
#include <assert.h>
#include "core/tree.hpp"
#include "io/print_in_log.hpp"

static FILE* log_file = stderr;

Node* create_node (LexType type, double value, Node* left, Node* right, TreeError* error)
{
    Node* new_node = (Node*) calloc (1, sizeof (Node));

    if (new_node == NULL)
    {
        *error = TREE_ERROR_CALLOC;
        return NULL;
    }

    new_node->elem.type = type;

    switch (type)
    {
        case LEX_TYPE_OPER:
            new_node->elem.elem.oper = (LexOperator) value;
            break;
        case LEX_TYPE_NUM:
            new_node->elem.elem.num = value;
            break;
        case LEX_TYPE_VAR:
            new_node->elem.elem.var_number = (size_t) value;
            break;
        case LEX_TYPE_DELIM:
            new_node->elem.elem.delim = (LexDelim) value;
            break;
        case LEX_TYPE_TXT:
        default:
            *error = TREE_ERROR_TYPE;
            return NULL;
    }

    new_node->left = left;
    new_node->right = right;

    return new_node;
}

void tree_dtor (Node* root)
{
    assert (root != NULL);

    if (root->left != NULL)
        tree_dtor (root->left);

    if (root->right != NULL)
        tree_dtor (root->right);

    free (root);
}

void tree_print_error (TreeError error)
{
    PRINT ("%s\n", tree_get_error (error));
}

const char* tree_get_error (TreeError error)
{
    switch (error)
    {
        case TREE_ERROR_OK:
            return "Tree: Ошибок не обнаружено.";
        case TREE_ERROR_CALLOC:
            return "Tree: Ошибка calloc.";
        case TREE_ERROR_TYPE:
            return "Tree: Ошибка типа.";
        default:
            return "Tree: Нужной ошибки не найдено...";
    }
}
