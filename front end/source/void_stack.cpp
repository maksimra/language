#include "../include/void_stack.hpp"
#include "../include/print_in_log.hpp"
#include "../include/compare_doubles.hpp"

#define STK_HASH  stk->stk_hash
#define DATA_HASH stk->data_hash
#define SIZE      stk->size
#define CAPACITY  stk->capacity
#define ELEM_SIZE stk->elem_size

static FILE* log_file = stderr;

const size_t DEFAULT_CAPACITY = 10;

const size_t CAPACITY_GROWTH_FACTOR = 4;

const double limit = 1e-6;

const int COEFF_REALLOC_DOWN = 4;

const can_type CANARY_MAIN = 0xBADDEDBADDEDBAD;

const uint32_t hash_seed = 0xABFDDCAE;

//==================================================================================================

void stack_set_log_file (FILE* file)
{
    log_file = file;
}

StkError stack_ctor (Stack *stk, size_t elem_size)
{
    PRINT_BEGIN();

    if (stk->data != NULL)
        return STK_ERROR_SECOND_CTOR;

    if (elem_size <= 0)
        return STK_ERROR_NEGATIVE_ELEM_SIZE;

    ELEM_SIZE = elem_size;

    stk_resize (stk, DEFAULT_CAPACITY);
    SIZE = 0;
    CAPACITY = DEFAULT_CAPACITY;

    stk->canary_left = CANARY_MAIN;
    stk->canary_right = CANARY_MAIN;

    STK_HASH = DATA_HASH = 0;
    STK_HASH = get_hash ((uint8_t*) stk, sizeof (Stack));
    DATA_HASH = get_hash ((uint8_t*) stk->data, CAPACITY * ELEM_SIZE);

    PRINT_END();
    return STK_NO_ERROR;
}

StkError stack_dtor (Stack *stk)
{
    PRINT_BEGIN();
    StkError error = STK_NO_ERROR;
    error = stk_verifier (stk);

    if (error == STK_ERROR_STR_HASH    ||
        error == STK_ERROR_LEFT_CANARY ||
        error == STK_ERROR_RIGHT_CANARY)
    {
        return error;
    }

    free ((char*) stk->data - sizeof(CANARY_MAIN));

    stk->data = NULL;
    CAPACITY = 0;
    SIZE = 0;
    ELEM_SIZE = 0;
    STK_HASH = DATA_HASH = 0;
    PRINT_END();
    return STK_NO_ERROR;
}

void stack_dump (Stack *stk)
{
    PRINT ("capacity = %zu\n"
           "size = %zu\n", CAPACITY, SIZE);
    PRINT ("left canary = 0x%jx\n",  *(can_type*)((char*) stk->data - sizeof (can_type)));
    PRINT ("right canary = 0x%jx\n", *(can_type*)((char*) stk->data + CAPACITY * ELEM_SIZE));
    PRINT ("struct hash = 0x%jx\n"
           "data hash = 0x%jx\n", STK_HASH, DATA_HASH);
}

uint32_t get_hash (const uint8_t* key, size_t length)
{
    size_t i = 0;
    uint32_t hash = 0;
    while (i != length)
    {
        hash += key[i++];
        hash += hash << 10;
        hash ^= hash >> 6;
    }
    hash += hash << 3;
    hash ^= hash >> 11;
    hash += hash << 15;
    return hash;
}

StkError stack_push (Stack *stk, const void* elem)
{
    PRINT_BEGIN();
    StkError error = STK_NO_ERROR;
    error = stk_verifier (stk);
    if (error != STK_NO_ERROR)
        return error;
    error = stk_realloc_up (stk);
    if (error != STK_NO_ERROR)
        return error;
    memcpy ((char*) stk->data + SIZE * ELEM_SIZE, elem, stk->elem_size);
    ++(SIZE);
    STK_HASH = DATA_HASH = 0;
    STK_HASH = get_hash ((uint8_t*) stk, sizeof (Stack));
    DATA_HASH = get_hash ((uint8_t*) stk->data, CAPACITY * ELEM_SIZE);
    PRINT_END();
    return STK_NO_ERROR;
}

StkError stk_realloc_up (Stack* stk)
{
    PRINT_BEGIN();
    if (SIZE >= CAPACITY)
    {
        return stk_resize (stk, CAPACITY + CAPACITY_GROWTH_FACTOR);
    }
    return STK_NO_ERROR;
}

StkError stk_resize (Stack* stk, size_t new_capacity)
{
    PRINT_BEGIN();
    void* temp = NULL;
    if (stk->data == NULL)
    {
        temp = calloc (1, new_capacity * ELEM_SIZE + 2 * sizeof (CANARY_MAIN));

        if (temp == NULL)
            return STK_ERROR_CALLOC;

        set_left_canary (temp);
        set_right_canary (stk, temp, new_capacity);
        set_data (stk, temp);
    }
    else
    {
        *(can_type*) ((char*) stk->data + CAPACITY * ELEM_SIZE) = 0;

        temp = realloc ((char*) stk->data - sizeof (CANARY_MAIN),
                                        new_capacity * ELEM_SIZE + 2 * sizeof (CANARY_MAIN));

        if (temp == NULL)
            return STK_ERROR_REALLOC;

        set_data (stk, temp);
        set_right_canary (stk, temp, new_capacity);
    }
    CAPACITY = new_capacity;
    PRINT_END();
    return STK_NO_ERROR;
}

void set_left_canary (void* new_ptr)
{
    *(can_type*) new_ptr = CANARY_MAIN;
}

void set_right_canary (Stack* stk, void* new_ptr, size_t new_capacity)
{
    *(can_type*)((char*) new_ptr + sizeof (CANARY_MAIN) + new_capacity * ELEM_SIZE) = CANARY_MAIN;
}

void set_data (Stack* stk, void* new_ptr)
{
    stk->data = (void*)((char*) new_ptr + sizeof (CANARY_MAIN));
}

void* stack_pop (Stack *stk, StkError* error)
{
    PRINT_BEGIN();
    *error = stk_verifier (stk);
    if (*error)
        return NULL;

    if (SIZE <= 0)
    {
        *error = STK_ERROR_POP;
        return NULL;
    }

    --(SIZE);
    *error = stk_realloc_down (stk);
    if (*error)
        return NULL;

    STK_HASH = DATA_HASH = 0;
    STK_HASH = get_hash ((uint8_t*) stk, sizeof (Stack));
    DATA_HASH = get_hash ((uint8_t*) stk->data, CAPACITY * ELEM_SIZE);
    PRINT_END();
    return (char*) stk->data + SIZE * ELEM_SIZE;
}

StkError stk_realloc_down (Stack *stk)
{
    PRINT_BEGIN();
    if (SIZE < CAPACITY / COEFF_REALLOC_DOWN)
    {
        return stk_resize (stk, CAPACITY / 2);
    }
    PRINT_END();
    return STK_NO_ERROR;
}

void stk_print_error (StkError error)
{
    PRINT ("%s\n", stk_get_error (error));
}

StkError stk_verifier (Stack* stk)
{
    PRINT_BEGIN();
    if (stk == NULL)
        return STK_ERROR_NULL_PTR_STK;

    if (CAPACITY <= 0)
        return STK_ERROR_NEGATIVE_CAPACITY;

    if (stk->data == NULL)
        return STK_ERROR_NULL_PTR_DATA;

    if (stk->canary_left != CANARY_MAIN)
        return STK_ERROR_LEFT_STR_CANARY;

    if (stk->canary_right != CANARY_MAIN)
        return STK_ERROR_RIGHT_STR_CANARY;

    if (SIZE > CAPACITY)
        return STK_ERROR_SIZE_BIGGER_CAPACITY;

    if (*((can_type*) stk->data - 1) != CANARY_MAIN)
        return STK_ERROR_LEFT_CANARY;

    if (*(can_type*) ((char*) stk->data + CAPACITY * ELEM_SIZE) != CANARY_MAIN)
        return STK_ERROR_RIGHT_CANARY;

    uint32_t prev_stk_hash = (uint32_t) STK_HASH;
    uint32_t prev_data_hash = (uint32_t) DATA_HASH;
    STK_HASH = DATA_HASH = 0;

    if (prev_stk_hash != get_hash ((uint8_t*) stk, sizeof (Stack)))
    {
        return STK_ERROR_STR_HASH;
    }

    if (prev_data_hash != get_hash ((uint8_t*) stk->data, CAPACITY * ELEM_SIZE))
    {
        return STK_ERROR_DATA_HASH;
    }

    STK_HASH = prev_stk_hash;
    DATA_HASH = prev_data_hash;

    return STK_NO_ERROR;

}

const char* stk_get_error (StkError error)
{
    switch (error)
    {
        case STK_NO_ERROR:
            return "Stack: Ошибок в работе функций не выявлено.";
        case STK_ERROR_REALLOC:
            return "Stack: Ошибка в работе функции realloc.";
        case STK_ERROR_POP:
            return "Stack: Ошибка в работе функции pop.";
        case STK_ERROR_CALLOC:
            return "Stack: Ошибка в выделении памяти (calloc).";
        case STK_ERROR_NEGATIVE_ELEM_SIZE:
            return "Stack: Отрицательное значение elem_size.";
        case STK_ERROR_SECOND_CTOR:
            return "Stack: Stack уже был создан.";
        case STK_ERROR_NULL_PTR_DATA:
            return "Stack: Нулевой указатель stk->data";
        case STK_ERROR_NULL_PTR_STK:
            return "Stack: Передан нулевой указатель на стэк.";
        case STK_ERROR_SIZE_BIGGER_CAPACITY:
            return "Stack: Size больше, чем capacity.";
        case STK_ERROR_NO_POISON:
            return "Stack: Пустые ячейки не POISON.";
        case STK_ERROR_POISON:
            return "Stack: Ячейка(-и) POISON.";
        case STK_ERROR_LEFT_CANARY:
            return "Stack: Кто-то съел левую канарейку.";
        case STK_ERROR_RIGHT_CANARY:
            return "Stack: Кто-то съел правую канарейку.";
        case STK_ERROR_LEFT_STR_CANARY:
            return "Stack: Испортили левую канарейку структуры.";
        case STK_ERROR_RIGHT_STR_CANARY:
            return "Stack: Испортили правую канарейку структуры.";
        case STK_ERROR_STR_HASH:
            return "Stack: Повреждён хэш структуры.";
        case STK_ERROR_DATA_HASH:
            return "Stack: Повреждён хэш data.";
        case STK_ERROR_NEGATIVE_CAPACITY:
            return "Stack: Отрицательное значение capacity.";
        default:
            return "Stack: Нужной ошибки не найдено...";
    }
}
