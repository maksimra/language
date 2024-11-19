#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../include/dyn_array.hpp"
#include "../include/print_in_log.hpp"

const size_t UP_COEFF = 2;

static FILE* log_file = stderr;

void dyn_array_set_log_file (FILE* file)
{
    log_file = file;
}

DynArrError dyn_array_ctor (Darray* darr, size_t capacity, size_t elem_size)
{
    PRINT_BEGIN();
    assert (darr);

    if (darr->data != NULL)
        return ARR_ERROR_SECOND_CTOR;

    if (capacity <= 0)
        return ARR_ERROR_NEGATIVE_CAPACITY;

    if (elem_size <= 0)
        return ARR_ERROR_NEGATIVE_ELEM_SIZE;

    darr->elem_size = elem_size;
    darr->size = 0;
    PRINT_END();
    return dyn_array_resize (darr, capacity);
}

DynArrError dyn_array_push (Darray* darr, void* element)
{
    PRINT_BEGIN();
    assert (darr);

    DynArrError error = dyn_array_verifier (darr);

    if (error)
        return error;

    if (element == NULL)
        return ARR_ERROR_NULL_ELEM;

    if (darr->size >= darr->capacity)
    {
        error = dyn_array_resize (darr, darr->capacity * UP_COEFF);
        if (error)
            return error;
    }

    memcpy ((char*) darr->data + darr->size * darr->elem_size,
            element,
            darr->elem_size);

    (darr->size)++;

    PRINT_END();
    return error;
}

void* dyn_array_get (const Darray* darr, size_t index)
{
    PRINT_BEGIN();
    assert (darr);

    DynArrError error = dyn_array_verifier (darr);
    dyn_array_print_error (error);

    if ((error) || (index >= darr->size))
        return NULL;

    return (char*) darr->data + index * darr->elem_size;
}

DynArrError dyn_array_resize (Darray* darr, size_t new_capacity)
{
    PRINT_BEGIN();
    assert (darr);

    void* temp = NULL;
    if (darr->data == NULL)
    {
        temp = calloc (new_capacity, darr->elem_size);

        if (temp == NULL)
            return ARR_ERROR_CALLOC;
    }
    else
    {
        temp = realloc (darr->data, new_capacity * darr->elem_size);
        if (temp == NULL)
            return ARR_ERROR_REALLOC;
    }
    darr->data = temp;
    darr->capacity = new_capacity;
    PRINT_END();
    return ARR_ERROR_OK;
}

DynArrError dyn_array_dtor (Darray* darr)
{
    PRINT_BEGIN();
    assert (darr);

    if (darr->data == NULL)
        return ARR_ERROR_NULL_PTR_DATA;
    free (darr->data);
    darr->size = 0;
    darr->capacity = 0;
    PRINT_END();
    return ARR_ERROR_OK;
}

DynArrError dyn_array_verifier (const Darray* darr)
{
    PRINT_BEGIN();
    if (darr == NULL)
        return ARR_ERROR_NULL_PTR_DARR;

    if (darr->capacity <= 0)
        return ARR_ERROR_NEGATIVE_CAPACITY;

    if (darr->data == NULL)
        return ARR_ERROR_NULL_PTR_DATA;

    if (darr->size > darr->capacity)
        return ARR_ERROR_SIZE_BIGGER_CAPACITY;

    PRINT_END();
    return ARR_ERROR_OK;
}

const char* dyn_array_get_error (DynArrError error)
{
    switch (error)
    {
        case ARR_ERROR_OK:
            return "Stack: Ошибок в работе функций не выявлено.";
        case ARR_ERROR_REALLOC:
            return "DynArr: Ошибка в работе функции realloc.";
        case ARR_ERROR_CALLOC:
            return "DynArr: Ошибка выделения памяти (calloc).";
        case ARR_ERROR_NEGATIVE_ELEM_SIZE:
            return "DynArr: Отрицательное значение elem_size.";
        case ARR_ERROR_SECOND_CTOR:
            return "DynArr: Массив уже был создан.";
        case ARR_ERROR_NULL_PTR_DATA:
            return "DynArr: Нулевой указатель data";
        case ARR_ERROR_NULL_PTR_DARR:
            return "DynArr: Передан нулевой указатель на массив.";
        case ARR_ERROR_SIZE_BIGGER_CAPACITY:
            return "DynArr: Size больше, чем capacity.";
        case ARR_ERROR_NULL_ELEM:
            return "DynArr: Нулевой указатель на элемент.";
        case ARR_ERROR_NEGATIVE_SIZE:
            return "DynArr: Size оказался отрицательным.";
        case ARR_ERROR_NEGATIVE_CAPACITY:
            return "DynArr: Отрицательный capacity.";
        default:
            return "DynArr: Нужной ошибки не найдено...";
    }
}

void dyn_array_print_error (DynArrError error)
{
    PRINT ("%s\n", dyn_array_get_error (error));
}
