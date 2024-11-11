#ifndef FRONT_END_HPP
#define FRONT_END_HPP

#include "void_stack.hpp"
#include "lexer.hpp"
// #include "build_tree.hpp"

enum FrontError
{
    FRONT_ERROR_OK             = 0,
    FRONT_ERROR_SETVBUF        = 1,
    FRONT_ERROR_NULL_PTR_SRUCT = 2,
    FRONT_ERROR_FOPEN          = 3,
    FRONT_ERROR_STK            = 4,
    FRONT_ERROR_CALLOC         = 5,
    FRONT_ERROR_PROC_FILE      = 6
};

struct FrontInfo
{
    FILE* input_file;
    size_t size_of_file;
    LexInfo* tokens;
    char* input_buffer;
    FILE* output_file;
    Stack vars;
};

void        frontend_set_log_file (FILE* file);
void        frontend_print_error  (FrontError error);
const char* frontend_get_error    (FrontError error);
FrontError  frontend_ctor         (FrontInfo* str, const char* name_of_input_file, const char* name_of_output_file);
FrontError  frontend_pass         (FrontInfo* str);
FrontError  frontend_dtor         (FrontInfo* str);

#endif // FRONT_END_HPP
