#include <stdio.h>
#include <assert.h>
#include "../include/parser.hpp"
#include "../include/lexer.hpp"
#include "../include/dyn_array.hpp"
#include "../include/print_in_log.hpp"

#define CUR_TYPE   ((LexInfo*)((char*) tokens->data + *n_tok * tokens->elem_size))->type
#define CUR_OPER   ((LexInfo*)((char*) tokens->data + *n_tok * tokens->elem_size))->elem.oper
#define CUR_SYMBOL ((LexInfo*)((char*) tokens->data + *n_tok * tokens->elem_size))->elem.symbol
#define CUR_NUMBER ((LexInfo*)((char*) tokens->data + *n_tok * tokens->elem_size))->elem.num
#define VAR_NUMBER ((LexInfo*)((char*) tokens->data + *n_tok * tokens->elem_size))->elem.var_number
#define PREV_OPER  ((LexInfo*)((char*) tokens->data + (*n_tok - 1) * tokens->elem_size))->elem.oper

static FILE* log_file = stderr;

Node* parse (const Darray* tokens, ParseError* error)
{
    size_t cur_token_number = 0;
    return get_assign (tokens, &cur_token_number, error);
}

Node* get_assign (const Darray* tokens, size_t* n_tok, ParseError* error)
{
    if (CUR_TYPE == LEX_TYPE_VAR)
    {
        size_t var_number = VAR_NUMBER;
        (*n_tok)++;
        if (CUR_TYPE == LEX_TYPE_OPER &&
            CUR_OPER == LEX_OPER_ASSIGN)
        {
            (*n_tok)++;
            return create_node (LEX_TYPE_OPER, LEX_OPER_ASSIGN,
                                create_node (LEX_TYPE_VAR, var_number, NULL, NULL, error),
                                get_expr (tokens, n_tok, error), error);
        }
    }
    return get_expr (tokens, n_tok, error);
}

Node* get_expr (const Darray* tokens, size_t* n_tok, ParseError* error)
{
    Node* val = NULL;
    if (CUR_TYPE == LEX_TYPE_OPER &&
        CUR_OPER == LEX_OPER_SUB)
    {
        (*n_tok)++;

        if (couple_oper (tokens, *n_tok) == true)
        {
            *error = PARSE_ERROR_SYNTAX;
            fprintf (stderr, "Введено некорректное выражение: оператор \"%s\" сразу после оператора \"%s\"."
                             "Работа программы завершена досрочно :(\n", OPERS[(int) CUR_OPER].name,
                                                                         OPERS[(int) PREV_OPER].name);
            return NULL;
        }

        val = create_node (LEX_TYPE_OPER, LEX_OPER_MUL,
                           create_node (LEX_TYPE_NUM, -1, NULL, NULL, error),
                           try_mult (tokens, n_tok, error), error);
    }
    else
    {
        val = try_mult (tokens, n_tok, error);
    }
    while (CUR_TYPE == LEX_TYPE_OPER &&
           (CUR_OPER == LEX_OPER_ADD || CUR_OPER == LEX_OPER_SUB))
    {
        (*n_tok)++;

        if (couple_oper (tokens, *n_tok) == true)// TODO: переименовать функцию
        {
            *error = PARSE_ERROR_SYNTAX;
            fprintf (stderr, "Введено некорректное выражение: оператор \"%s\" сразу после оператора \"%s\"."
                             "Работа программы завершена досрочно :(\n", OPERS[(int) CUR_OPER].name,
                                                                         OPERS[(int) PREV_OPER].name);
            tree_dtor (val); // TODO: вспомнить, для чего это
            return NULL;
        }

        Node* val2 = try_mult (tokens, n_tok, error);
        if (PREV_OPER == LEX_OPER_ADD)
            val = create_node (LEX_TYPE_OPER, LEX_OPER_ADD, val, val2, error);
        else
            val = create_node (LEX_TYPE_OPER, LEX_OPER_SUB, val, val2, error);
    }
    return val;
}

Node* try_pow (const Darray* tokens, size_t* n_tok, ParseError* error)
{
    Node* val = try_parenthesis (tokens, n_tok, error);
    if (CUR_TYPE == LEX_TYPE_OPER && CUR_OPER == LEX_OPER_POW)
    {
        (*n_tok)++;

        if (couple_oper (tokens, *n_tok) == true)
        {
            *error = PARSE_ERROR_SYNTAX;
            fprintf (stderr, "Введено некорректное выражение: оператор \"%s\" сразу после оператора \"%s\"."
                             "Работа программы завершена досрочно :(\n", OPERS[(int) CUR_OPER].name,
                                                                         OPERS[(int) PREV_OPER].name);
            tree_dtor (val);
            return NULL;
        }

        Node* val2 = try_parenthesis (tokens, n_tok, error);
        val = create_node (LEX_TYPE_OPER, LEX_OPER_POW, val, val2, error);

        if (CUR_TYPE == LEX_TYPE_OPER && CUR_OPER == LEX_OPER_POW)
        {
            *error = PARSE_ERROR_SYNTAX;
            fprintf (stderr, "Введено некорректное выражение: "
                             "используйте скобки для записи.\n");
        }
    }
    return val;
}

Node* try_func (const Darray* tokens, size_t* n_tok, ParseError* error)
{
    if (CUR_TYPE == LEX_TYPE_OPER)
    {
        int n_oper = CUR_OPER;
        (*n_tok)++;

        if (CUR_TYPE == LEX_TYPE_OPER)
        {
            *error = PARSE_ERROR_SYNTAX;
            fprintf (stderr, "Введено некорректное выражение: оператор \"%s\" сразу после оператора \"%s\"."
                             "Работа программы завершена досрочно :(\n", OPERS[(int) CUR_OPER].name,
                                                                         OPERS[(int) PREV_OPER].name);
            return NULL;
        }

        if (CUR_TYPE == LEX_TYPE_TXT && CUR_SYMBOL != '(')
        {
            *error = PARSE_ERROR_SYNTAX;
            fprintf (stderr, "Введено некорректное выражение:"
                             "после оператора %s ожидается '(' "
                             "(пробелы допускаются).\n", OPERS[(int) CUR_OPER].name);
            return NULL;
        }

        (*n_tok)++;
        Node* val = get_expr (tokens, n_tok, error);

        if (CUR_TYPE == LEX_TYPE_TXT && CUR_SYMBOL != ')')
            *error = PARSE_ERROR_SYNTAX;

        (*n_tok)++;

        switch (OPERS[n_oper].op_enum)
        {
            case LEX_OPER_SIN:
                val = create_node (LEX_TYPE_OPER, LEX_OPER_SIN,  NULL, val, error);
                return val;
            case LEX_OPER_COS:
                val = create_node (LEX_TYPE_OPER, LEX_OPER_COS,  NULL, val, error);
                return val;
            case LEX_OPER_LN:
                val = create_node (LEX_TYPE_OPER, LEX_OPER_LN,   NULL, val, error);
                return val;
            case LEX_OPER_SQRT:
                val = create_node (LEX_TYPE_OPER, LEX_OPER_SQRT, NULL, val, error);
                return val;
            case LEX_OPER_EXP:
                val = create_node (LEX_TYPE_OPER, LEX_OPER_EXP,  NULL, val, error);
                return val;
            case LEX_OPER_ADD:
            case LEX_OPER_SUB:
            case LEX_OPER_MUL:
            case LEX_OPER_DIV:
            case LEX_OPER_POW:
            case LEX_OPER_NONE:
            default:
                *error = PARSE_ERROR_SYNTAX;
        }
    }
    return try_pow (tokens, n_tok, error);
}

Node* try_parenthesis (const Darray* tokens, size_t* n_tok, ParseError* error)
{
    if (CUR_TYPE == LEX_TYPE_TXT && CUR_SYMBOL == '(')
    {
        (*n_tok)++;
        Node* val = get_expr (tokens, n_tok, error);

        if (CUR_TYPE == LEX_TYPE_TXT && CUR_SYMBOL != ')')
            *error = PARSE_ERROR_SYNTAX;

        (*n_tok)++;

        return val;
    }
    Node* val = try_item (tokens, n_tok, error);
    return val;
}

Node* try_mult (const Darray* tokens, size_t* n_tok, ParseError* error)
{
    Node* val = try_func (tokens, n_tok, error);
    while (CUR_TYPE == LEX_TYPE_OPER &&
           (CUR_OPER == LEX_OPER_MUL || CUR_OPER == LEX_OPER_DIV))
    {
        (*n_tok)++;

        if (couple_oper (tokens, *n_tok) == true)
        {
            *error = PARSE_ERROR_SYNTAX;
            fprintf (stderr, "Введено некорректное выражение: оператор \"%s\" сразу после оператора \"%s\"."
                             "Работа программы завершена досрочно :(\n", OPERS[(int) CUR_OPER].name,
                                                                         OPERS[(int) PREV_OPER].name);
            tree_dtor (val); // TODO: вспомнить для чего
            return NULL;
        }

        Node* val2 = try_func (tokens, n_tok, error);

        if (PREV_OPER == LEX_OPER_MUL)
            val = create_node (LEX_TYPE_OPER, LEX_OPER_MUL, val, val2, error);
        else
            val = create_node (LEX_TYPE_OPER, LEX_OPER_DIV, val, val2, error);
    }
    return val;
}

Node* try_item (const Darray* tokens, size_t* n_tok, ParseError* error)
{
    double val = 0;
    int n_var = -1;

    if (CUR_TYPE == LEX_TYPE_NUM)
    {
        val = CUR_NUMBER;
        (*n_tok)++;
        return create_node (LEX_TYPE_NUM, val, NULL, NULL, error);
    }
    else if (CUR_TYPE == LEX_TYPE_VAR)
    {
        n_var = VAR_NUMBER;
        (*n_tok)++;
        return create_node (LEX_TYPE_VAR, n_var, NULL, NULL, error);
    }
    else
    {
        *error = PARSE_ERROR_SYNTAX;
        return NULL;
    }
}

bool couple_oper (const Darray* tokens, size_t n_tok_)
{
    size_t* n_tok = &n_tok_; // TODO: стрёмный костыль
    if (CUR_TYPE == LEX_TYPE_OPER)
        if (!OPERS[(int) CUR_OPER].is_func)
            return true;

    return false;
}

Node* create_node (LexType type, double value, Node* left, Node* right, ParseError* error)
{
    Node* new_node = (Node*) calloc (1, sizeof (Node));

    if (new_node == NULL)
    {
        *error = PARSE_ERROR_CALLOC;
        return NULL;
    }

    new_node->elem->type = type;

    if (type == LEX_TYPE_OPER)
        new_node->elem->elem.oper = (LexOperator) value;

    else if (type == LEX_TYPE_NUM)
        new_node->elem->elem.num = value;

    else if (type == LEX_TYPE_VAR)
        new_node->elem->elem.var_number = (int) value;

    new_node->left = left;
    new_node->right = right;

    return new_node;
}

void tree_dtor (Node* root)
{
    assert (root != NULL);

    if (root->left != NULL)
        tree_dtor (root->left);

    if (root->right != NULL)
        tree_dtor (root->right);

    free (root);
}

void parse_set_log_file (FILE* file)
{
    log_file = file;
}

void parse_print_error (ParseError error)
{
    PRINT ("%s\n", parse_get_error (error));
}

const char* parse_get_error (ParseError error)
{
    switch (error)
    {
        case PARSE_ERROR_OK:
            return "Parse: Ошибок не обнаружено.";
        case PARSE_ERROR_SYNTAX:
            return "Parse: Синтаксическая ошибка.";
        case PARSE_ERROR_CALLOC:
            return "Parse: Ошибка calloc.";
        default:
            return "Parse: Нужной ошибки не найдено...";
    }
}
