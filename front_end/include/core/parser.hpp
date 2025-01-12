#ifndef PARSER_HPP
#define PARSER_HPP

#include "lexer.hpp"
#include "tree.hpp"

enum ParseError
{
    PARSE_ERROR_OK     = 0,
    PARSE_ERROR_SYNTAX = 1,
    PARSE_ERROR_CALLOC = 2,
    PARSE_ERROR_FWRITE = 3,
    PARSE_ERROR_TREE   = 4
};

Node*       parse              (const Darray* tokens, ParseError* error);
Node*       get_assign         (const Darray* tokens, size_t* n_tok, ParseError* error);
Node*       get_expr           (const Darray* tokens, size_t* n_tok, ParseError* error);
Node*       try_pow            (const Darray* tokens, size_t* n_tok, ParseError* error);
Node*       try_func           (const Darray* tokens, size_t* n_tok, ParseError* error);
Node*       try_parenthesis    (const Darray* tokens, size_t* n_tok, ParseError* error);
Node*       try_mult           (const Darray* tokens, size_t* n_tok, ParseError* error);
Node*       try_item           (const Darray* tokens, size_t* n_tok, ParseError* error);
bool        couple_oper        (const Darray* tokens, size_t* n_tok);
ParseError  write_tree_in_file (const Node* root, char* text_tree, FILE* output_file);
ParseError  get_text_tree      (const Node* node, char* text_tree, size_t* text_size);
void        parse_set_log_file (FILE* file);
void        parse_print_error  (ParseError error);
const char* parse_get_error    (ParseError error);


#endif // PARSER_HPP
