#include "../include/skip_space.hpp"
#include <ctype.h>

void skip_space (char** str)
{
    while (isspace(**str))
        (*str)++;
}
