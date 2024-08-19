#include "../include/compare_doubles.hpp"
#include <math.h>

const double ACCURACY = 1e-5;

int compare_doubles (double a, double b)
{
    return (a > b + ACCURACY) - (b > a + ACCURACY);
}
