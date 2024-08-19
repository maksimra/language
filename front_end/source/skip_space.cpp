#include "../include/skip_space.hpp"
#include <ctype.h>

void skip_space (const char** str)
{
    while (isspace(**str))
        (*str)++;
}
