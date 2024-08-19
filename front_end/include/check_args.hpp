#ifndef CHECK_ARGS_HPP
#define CHECK_ARGS_HPP

#include <stdio.h>

enum ArgsError
{
    ARGS_NO_ERROR   = 0,
    ARGS_ERROR_ARGC = 1,
    ARGS_ERROR_ARGV = 2
};

void           args_set_log_file   (FILE* file);
ArgsError      args_check          (const int argc, const char** argv, int ref_argc);
bool           args_print_if_error (ArgsError error);
const char*    args_get_error      (ArgsError error);

#endif // CHECK_ARGS_HPP
