#include <stdio.h>
#include "include/check_args.hpp"
#include "include/front_end_struct.hpp"

int main (const int argc, const char* argv[])
{
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
        fclose (log_file);
        return 1;
    }

    FrontError front_error = FRONT_ERROR_OK;
    Frontend str = {};
    const char* name_of_input_file  = argv[1];
    const char* name_of_output_file = argv[2];
    front_error = frontend_ctor (&str, name_of_input_file, name_of_output_file);
    if (front_error)
    {
        frontend_print_error (front_error);
        fprintf (stderr, "Error initializing FrontStruct.\n");
        fclose (log_file);
        return 1;
    }

    front_error = frontend_pass (&str);
    if (front_error)
    {
        frontend_print_error (front_error);
        fprintf (stderr, "Frontend error.\n");
        frontend_dtor (&str);
        fclose (log_file);
        return 1;
    }

    front_error = frontend_dtor (&str);
    if (front_error)
    {
        frontend_print_error (front_error);
        fprintf (stderr, "Error dtor FrontStruct.\n");
        fclose (log_file);
        return 1;
    }

    fclose (log_file);
    return 0;
}
