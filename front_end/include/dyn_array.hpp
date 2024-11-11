#ifndef DYN_ARRAY_HPP
#define DYN_ARRAY_HPP

#include "assembler.hpp"

enum DynArrError
{
    ARR_ERROR_OK            = 0,
    ARR_ERROR_SECOND_CTOR   = 1,
    ARR_ERROR_NEGATIVE_SIZE = 2,
    ARR_ERROR_NULL_PTR_DARR = 3,
    ARR_ERROR_NULL_PTR_DATA = 4,
    ARR_ERROR_CALLOC        = 5,
    ARR_ERROR_REALLOC       = 6
};

struct Darray
{
    void* data;
    size_t size;
    size_t capacity;
};

DynArrError dyn_array_ctor   (Darray* darr, size_t capacity);
DynArrError dyn_array_dtor   (Darray* darr);
DynArrError dyn_array_resize (Darray* darr, size_t new_capacity);
DynArrError dyn_array_push   (Darray* darr, darray_elem element);


#endif // DYN_ARRAY_HPP
