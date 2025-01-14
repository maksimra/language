#include <stdio.h>
#include <stdlib.h>
#include "../include/check_args.hpp"
#include "../include/back_end.hpp"

int main (const int argc, const char* argv[])
{
    int exit_code = EXIT_SUCCESS;

    FILE* log_file = fopen ("log_file.txt", "w");
    if (log_file == NULL)
        printf ("log_file wasn't open.\n");

    int return_value = setvbuf (log_file, NULL, _IONBF, 0);
    if (return_value)
        backend_print_error (BACK_ERROR_SETVBUF);

    args_set_log_file      (log_file);
    backend_set_log_file   (log_file);
    dyn_array_set_log_file (log_file);

    const int necessary_n_args = 3;
    ArgsError args_error = args_check (argc, necessary_n_args);
    if (args_print_if_error (args_error))
    {
        exit_code = EXIT_FAILURE;
        fclose (log_file);
        return exit_code;
    }

    BackError back_error = BACK_ERROR_OK;
    BackInfo back = {};
    const char* name_of_input_file  = argv[1];
    const char* name_of_output_file = argv[2];
    back_error = backend_ctor (&back, name_of_input_file, name_of_output_file);
    if (back_error)
    {
        backend_print_error (back_error);
        fprintf (stderr, "Error initializing BackStruct.\n");
        exit_code = EXIT_FAILURE;
        goto termination;
    }

    back_error = backend_pass (&back);
    if (back_error)
    {
        backend_print_error (back_error);
        fprintf (stderr, "Backend error.\n");
        backend_dtor (&back);
        exit_code = EXIT_FAILURE;
        goto termination;
    }

    back_error = backend_dtor (&back);
    if (back_error)
    {
        backend_print_error (back_error);
        fprintf (stderr, "Error dtor BackStruct.\n");
        exit_code = EXIT_FAILURE;
        goto termination;
    }

termination:
    fclose (log_file);
    return exit_code;
}
