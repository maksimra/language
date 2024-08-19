#ifndef FILE_PROCESSING_HPP
#define FILE_PROCESSING_HPP

enum ProcFileError
{
    PROC_FILE_NO_ERROR          = 0,
    PROC_FILE_ERROR_STAT        = 1,
    PROC_FILE_ERROR_FOPEN       = 2,
    PROC_FILE_ERROR_FREAD       = 3,
    PROC_FILE_ERROR_CALLOC      = 4,
    PROC_FILE_ERROR_PTR_TO_FILE = 5
};

void          proc_file_set_log_file   (FILE* file);
ProcFileError process_file             (char*** ptr_to_lines, const char* name_of_file, FILE* file, size_t* number_lines);
ProcFileError read_file_count_size     (const char* name_of_file, FILE* file, size_t* size, char** buffer);
size_t        line_processing          (size_t size, char* buffer);
ProcFileError make_ptr_to_lines        (size_t number_lines, char* buffer, char*** lines);
void          proc_file_print_error    (ProcFileError error);
const char*   proc_file_get_error      (ProcFileError error);

#endif // FILE_PROCESSING_HPP
