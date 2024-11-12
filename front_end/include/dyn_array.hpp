#ifndef DYN_ARRAY_HPP
#define DYN_ARRAY_HPP

#include <stdlib.h>

enum DynArrError
{
    ARR_ERROR_OK                   = 0,
    ARR_ERROR_SECOND_CTOR          = 1,
    ARR_ERROR_NEGATIVE_SIZE        = 2,
    ARR_ERROR_NULL_PTR_DARR        = 3,
    ARR_ERROR_NULL_PTR_DATA        = 4,
    ARR_ERROR_CALLOC               = 5,
    ARR_ERROR_REALLOC              = 6,
    ARR_ERROR_NEGATIVE_ELEM_SIZE   = 7,
    ARR_ERROR_NEGATIVE_CAPACITY    = 8,
    ARR_ERROR_SIZE_BIGGER_CAPACITY = 9,
    ARR_ERROR_NULL_ELEM            = 10
};

struct Darray
{
    void* data;
    size_t size;
    size_t capacity;
    size_t elem_size;
};

DynArrError dyn_array_ctor         (Darray* darr, size_t capacity, size_t elem_size);
DynArrError dyn_array_dtor         (Darray* darr);
DynArrError dyn_array_resize       (Darray* darr, size_t new_capacity);
DynArrError dyn_array_push         (Darray* darr, void* element);
DynArrError dyn_array_verifier     (Darray* darr);
const char* dyn_array_get_error    (DynArrError error);
void        dyn_array_print_error  (DynArrError error);
void        dyn_array_set_log_file (FILE* file);

#endif // DYN_ARRAY_HPP
