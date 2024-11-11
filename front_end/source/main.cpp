#include <stdio.h>
#include "../include/check_args.hpp"
#include "../include/front_end.hpp"

int main (const int argc, const char* argv[])
{
    int exit_code = EXIT_SUCCESS;

    FILE* log_file = fopen ("log_file.txt", "w");
    if (log_file == NULL)
        printf ("log_file wasn't open.\n");

    int return_value = setvbuf (log_file, NULL, _IONBF, 0);
    if (return_value)
    {
        frontend_print_error (FRONT_ERROR_SETVBUF);
    }

    args_set_log_file     (log_file);
    frontend_set_log_file (log_file);

    const int necessary_n_args = 2;
    ArgsError args_error = args_check (argc, argv, necessary_n_args);
    if (args_print_if_error (args_error))
    {
        exit_code = EXIT_FAILURE;
        fclose (log_file);
        return exit_code;
    }

    FrontError front_error = FRONT_ERROR_OK;
    FrontInfo front = {};
    const char* name_of_input_file  = argv[1];
    const char* name_of_output_file = argv[2];
    front_error = frontend_ctor (&front, name_of_input_file, name_of_output_file);
    if (front_error)
    {
        frontend_print_error (front_error);
        fprintf (stderr, "Error initializing FrontStruct.\n");
        exit_code = EXIT_FAILURE;
        goto termination;
    }

    front_error = frontend_pass (&front);
    if (front_error)
    {
        frontend_print_error (front_error);
        fprintf (stderr, "Frontend error.\n");
        frontend_dtor (&front);
        exit_code = EXIT_FAILURE;
        goto termination;
    }

    front_error = frontend_dtor (&front);
    if (front_error)
    {
        frontend_print_error (front_error);
        fprintf (stderr, "Error dtor FrontStruct.\n");
        exit_code = EXIT_FAILURE;
        goto termination;
    }

termination:
    fclose (log_file);
    return exit_code;
}
