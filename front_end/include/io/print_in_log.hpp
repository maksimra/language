#define PRINT_BEGIN() fprintf (log_file, "begin: %s.\n",      __PRETTY_FUNCTION__)
#define PRINT_END()   fprintf (log_file, "success end: %s\n", __PRETTY_FUNCTION__)
#define PRINT(...) if (log_file != NULL) fprintf (log_file, __VA_ARGS__)
