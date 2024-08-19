#ifndef BUILD_TREE_HPP
#define BUILD_TREE_HPP

#include "tokenizer.hpp"

enum TreeError
{
    TREE_ERROR_OK = 0;
};

struct Node
{
    TokenType type;
    TokenElem elem;

    Node* left;
    Node* right;
};

#endif // BUILD_TREE_HPP
