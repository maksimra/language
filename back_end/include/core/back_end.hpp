#ifndef BACK_END_HPP
#define BACK_END_HPP

enum BackError
{
    BACK_ERROR_OK             = 0,
    BACK_ERROR_FOPEN          = 2,
    BACK_ERROR_PROC_FILE      = 3,
    BACK_ERROR_SSCANF         = 4,
    BACK_ERROR_READ           = 5,
    BACK_ERROR_SEMANTICS      = 6
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
FrontError  backend_ctor         (BackInfo* back, const char* name_of_input_file);
FrontError  backend_pass         (BackInfo* back);
FrontError  backend_dtor         (BackInfo* back);

#endif // BACK_END_HPP
