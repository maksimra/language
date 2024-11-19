#ifndef PARSER_HPP
#define PARSER_HPP

#include "lexer.hpp"

enum ParseError
{
    PARSE_ERROR_OK     = 0,
    PARSE_ERROR_SYNTAX = 1,
    PARSE_ERROR_CALLOC
};

struct Node
{
    // LexType type; // TODO: узнать, что лучше, так или по указателю
    // LexElem elem;
    LexInfo elem;

    Node* left;
    Node* right;
};

Node*       parse              (const Darray* tokens, ParseError* error);
Node*       get_assign         (const Darray* tokens, size_t* n_tok, ParseError* error);
Node*       get_expr           (const Darray* tokens, size_t* n_tok, ParseError* error);
Node*       try_pow            (const Darray* tokens, size_t* n_tok, ParseError* error);
Node*       try_func           (const Darray* tokens, size_t* n_tok, ParseError* error);
Node*       try_parenthesis    (const Darray* tokens, size_t* n_tok, ParseError* error);
Node*       try_mult           (const Darray* tokens, size_t* n_tok, ParseError* error);
Node*       create_node        (LexType type, double value, Node* left, Node* right, ParseError* error);
void        tree_dtor          (Node* root); // TODO: спросить, может в другой файл инструменты для работы с деревом
Node*       try_item           (const Darray* tokens, size_t* n_tok, ParseError* error);
bool        couple_oper        (const Darray* tokens, size_t n_tok);
void        parse_set_log_file (FILE* file);
void        parse_print_error  (ParseError error);
const char* parse_get_error    (ParseError error);


#endif // PARSER_HPP
