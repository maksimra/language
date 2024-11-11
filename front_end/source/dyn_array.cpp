#include "../include/dyn_array.hpp"
#include "../include/print_in_log.hpp"

DynArrError dyn_array_ctor (Darray* darr, size_t capacity)
{
    PRINT_BEGIN();
    if (darr != NULL)
        return ARR_ERROR_SECOND_CTOR;

    if (capacity <= 0)
        return ARR_ERROR_NEGATIVE_SIZE;

    dyn_array_resize (darr, capacity);
    darr->size = 0;
    PRINT_END();
    return ARR_ERROR_OK;
}

DynArrError dyn_array_push (Darray* darr, darray_elem element)
{
    PRINT_BEGIN();
    DynArrError error = ARR_ERROR_OK;

    if (darr->size >= darr->capacity)
        error = dyn_array_resize (darr, darr->capacity * 2);

    if (error)
        return error;

    (darr->data)[darr->size] = element;
    (darr->size)++;

    PRINT_END();
    return error;
}

DynArrError dyn_array_resize (Darray* darr, size_t new_capacity)
{
    PRINT_BEGIN();
    darray_elem* temp = NULL;
    if (darr->data == NULL)
    {
        temp = (darray_elem*) calloc (sizeof (darray_elem), new_capacity);

        if (temp == NULL)
            return ARR_ERROR_CALLOC;
    }
    else
    {
        temp = (darray_elem*) realloc (darr->data, new_capacity);
        if (temp == NULL)
            return ARR_ERROR_REALLOC;
    }
    darr->data = temp;
    darr->size = new_capacity;
    PRINT_END();
    return ARR_ERROR_OK;
}

DynArrError dyn_array_dtor (Darray* darr)
{
    PRINT_BEGIN();
    if (darr == NULL)
        return ARR_ERROR_NULL_PTR_DARR;
    if (darr->data == NULL)
        return ARR_ERROR_NULL_PTR_DATA;
    free (darr->data);
    darr->size = 0;
    darr-capacity = 0;
    PRINT_END();
    return ARR_ERROR_OK;
}
