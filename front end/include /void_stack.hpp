#ifndef VOID_STACK_HPP
#define VOID_STACK_HPP
#define STK_CANARY_PROTECTION

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>

#ifdef STK_CANARY_PROTECTION
    #define STK_ON_CANARY_PROTECTION(...)  __VA_ARGS__
#else
    #define STK_ON_CANARY_PROTECTION(...)
#endif

typedef uint64_t can_type;

enum StkError
{
    STK_NO_ERROR                   = 0,
    STK_ERROR_REALLOC              = 1,
    STK_ERROR_POP                  = 2,
    STK_ERROR_CALLOC               = 3,
    STK_ERROR_NEGATIVE_ELEM_SIZE   = 4,
    STK_ERROR_SECOND_CTOR          = 5,
    STK_ERROR_NULL_PTR_STK         = 6,
    STK_ERROR_NULL_PTR_DATA        = 7,
    STK_ERROR_SIZE_BIGGER_CAPACITY = 8,
    STK_ERROR_NO_POISON            = 9,
    STK_ERROR_POISON               = 10,
    STK_ERROR_LEFT_CANARY          = 11,
    STK_ERROR_RIGHT_CANARY         = 12,
    STK_ERROR_LEFT_STR_CANARY      = 13,
    STK_ERROR_RIGHT_STR_CANARY     = 14,
    STK_ERROR_DATA_HASH            = 15,
    STK_ERROR_STR_HASH             = 16,
    STK_ERROR_NEGATIVE_CAPACITY    = 17
};

struct Stack
{
    STK_ON_CANARY_PROTECTION(can_type canary_left;)

    void* data;
    size_t size;
    size_t capacity;
    size_t elem_size;
    uint64_t stk_hash;
    uint64_t data_hash;

    STK_ON_CANARY_PROTECTION(can_type canary_right;)
};

StkError    stack_ctor         (Stack *stk, size_t elem_size);
StkError    stack_dtor         (Stack *stk);
StkError    stack_push         (Stack *stk, const void* elem);
void*       stack_pop          (Stack *stk, StkError* error);
void        stk_print_error    (StkError error);
void        stack_set_log_file (FILE* file);
StkError    stk_resize         (Stack* stk, size_t new_capacity);
const char* stk_get_error      (StkError error);
StkError    stk_verifier       (Stack* stk);
StkError    stk_realloc_down   (Stack *stk);
StkError    stk_realloc_up     (Stack* stk);
uint32_t    get_hash           (const uint8_t* key, size_t length);
void        stack_dump         (Stack *stk);
void        set_left_canary    (void* new_ptr);
void        set_right_canary   (Stack* stk, void* new_ptr, size_t new_capacity);
void        set_data           (Stack* stk, void* new_ptr);

#endif // VOID_STACK_HPP
