#ifndef BACK_END_HPP
#define BACK_END_HPP

#include "tree.hpp"

enum BackError
{
    BACK_ERROR_OK        = 0,
    BACK_ERROR_FOPEN     = 2,
    BACK_ERROR_PROC_FILE = 3,
    BACK_ERROR_SSCANF    = 4,
    BACK_ERROR_READ      = 5,
    BACK_ERROR_SEMANTICS = 6,
    BACK_ERROR_NODE_TYPE = 7,
    BACK_ERROR_TREE      = 8,
    BACK_ERROR_SETVBUF   = 9
};

struct BackInfo
{
    FILE* input_file;
    size_t size_of_file;
    char* input_buffer;
    FILE* output_file;
    Node* root;
};

void        backend_set_log_file (FILE* file);
void        backend_print_error  (BackError error);
const char* backend_get_error    (BackError error);
BackError   backend_ctor         (BackInfo* back, const char* name_of_input_file, const char* name_of_output_file);
BackError   backend_pass         (BackInfo* back);
Node*       create_tree          (char** buffer, const char* buf_end, BackError* error);
BackError   create_asm_file      (FILE* output_file, Node* node, Node* parent);
void        backend_dtor         (BackInfo* back);

#endif // BACK_END_HPP
