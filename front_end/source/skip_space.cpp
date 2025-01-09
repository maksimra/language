#include <ctype.h>
#include <stdio.h>
#include <stdbool.h>
#include "../include/skip_space.hpp"

bool skip_space(char **str)
{
    if (**str == '\0')
        return true;

    while (isspace(**str))
        (*str)++;
    return false;
}
