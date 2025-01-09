#ifndef TREE_HPP
#define TREE_HPP

#include "lexer.hpp"

enum TreeError
{
    TREE_ERROR_OK     = 0,
    TREE_ERROR_CALLOC = 1
};

struct Node
{
    LexInfo elem;

    Node* left;
    Node* right;
};

void        tree_dtor        (Node* root);
Node*       create_node      (LexType type, double value, Node* left, Node* right, TreeError* error);
void        tree_print_error (TreeError error);
const char* tree_get_error   (TreeError error);

#endif // TREE_HPP
