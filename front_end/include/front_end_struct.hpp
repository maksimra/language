#ifndef FRONT_END_STRUCT_HPP
#define FRONT_END_STRUCT_HPP

#include "void_stack.hpp"
#include "build_tree.hpp"

enum FrontError
{
    FRONT_ERROR_OK             = 0,
    FRONT_ERROR_SETVBUF        = 1,
    FRONT_ERROR_NULL_PTR_SRUCT = 2,
    FRONT_ERROR_FOPEN          = 3,
    FRONT_ERROR_STK            = 4
};

struct Frontend
{
    FILE* input_file;
    char* input_buffer;
    char* output_buffer ;
    FILE* output_file;
    Stack vars;
    Node* root;
};

void        frontend_set_log_file (FILE* file);
void        frontend_print_error  (FrontError error);
const char* frontend_get_error    (FrontError error);
FrontError  frontend_ctor         (Frontend* str, const char* name_of_input_file, const char* name_of_output_file);
FrontError  frontend_pass         (Frontend* str);
FrontError  frontend_dtor         (Frontend* str);

#endif // FRONT_END_STRUCT_HPP
