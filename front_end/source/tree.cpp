#include <stdio.h>
#include "../include/tree.hpp"

static FILE* file = stderr;

Node* create_node (LexType type, double value, Node* left, Node* right, ParseError* error)
{
    Node* new_node = (Node*) calloc (1, sizeof (Node));

    if (new_node == NULL)
    {
        *error = PARSE_ERROR_CALLOC;
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
        case LEX_TYPE_TXT:
        default:
            *error = PARSE_ERROR_SYNTAX;
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
        default:
            return "Tree: Нужной ошибки не найдено...";
    }
}
