#include <stdio.h>
#include <assert.h>
#include <cstdint>
#include <cstring>
#include "../include/parser.hpp"
#include "../include/lexer.hpp"
#include "../include/dyn_array.hpp"
#include "../include/print_in_log.hpp"

#define CUR_TYPE   ((LexInfo*) tokens->data + *n_tok)->type
#define CUR_OPER   ((LexInfo*) tokens->data + *n_tok)->elem.oper
#define CUR_DEL    ((LexInfo*) tokens->data + *n_tok)->elem.delim
#define CUR_SYMBOL ((LexInfo*) tokens->data + *n_tok)->elem.symbol
#define CUR_NUMBER ((LexInfo*) tokens->data + *n_tok)->elem.num
#define VAR_NUMBER ((LexInfo*) tokens->data + *n_tok)->elem.var_number
#define PREV_OPER  ((LexInfo*) tokens->data + *n_tok - 1)->elem.oper
#define PREV_TYPE  ((LexInfo*) tokens->data + *n_tok - 1)->type

static FILE* log_file = stderr;

Node* parse (const Darray* tokens, ParseError* error)
{
    Node* root = NULL;
    Node* val = NULL;
    size_t cur_token_number = 0;
    size_t* n_tok = &cur_token_number; // TODO: подумать, как убрать
    do
    {
        Node* node = get_assign (tokens, &cur_token_number, error);
        if (CUR_TYPE != LEX_TYPE_DELIM && CUR_DEL != LEX_DEL_SEMICOLON)
        {
            *error = PARSE_ERROR_SYNTAX;

            val->right = node; // Привязываю для правильной очистки
            return root;
        }

        cur_token_number++;

        TreeError tree_error = TREE_ERROR_OK;
        Node* semicolon_node = create_node (LEX_TYPE_DELIM, LEX_DEL_SEMICOLON,
                                            node,
                                            NULL, &tree_error);
        if (tree_error)
        {
            tree_print_error (tree_error);
            *error = PARSE_ERROR_TREE;
            return root;
        } // TODO: в макрос обернуть

        if (val != NULL)
        {
            val->right = semicolon_node; // TODO: потом насчёт обработки ошибок подумать
            val = val->right;
        }
        else
        {
            val = semicolon_node;
            root = val;
        }
    } while (tokens->size > cur_token_number);

    return root;
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

            TreeError tree_error = TREE_ERROR_OK;
            Node* val = create_node (LEX_TYPE_OPER, (double) LEX_OPER_ASSIGN,
                                     create_node (LEX_TYPE_VAR, (double) var_number, NULL, NULL, &tree_error),
                                     get_expr (tokens, n_tok, error), &tree_error);

            if (tree_error)
            {
                tree_print_error (tree_error);
                *error = PARSE_ERROR_TREE;
                return NULL; // TODO: макрос!!!
            }

            return val;
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

        if (couple_oper (tokens, n_tok) == true)
        {
            *error = PARSE_ERROR_SYNTAX;
            fprintf (stderr, "Введено некорректное выражение: оператор \"%s\" сразу после оператора \"%s\"."
                             "Работа программы завершена досрочно :(\n", OPERS[(int) CUR_OPER].name,
                                                                         OPERS[(int) PREV_OPER].name);
            return NULL;
        }

        TreeError tree_error = TREE_ERROR_OK;;
        val = create_node (LEX_TYPE_OPER, LEX_OPER_MUL,
                           create_node (LEX_TYPE_NUM, -1, NULL, NULL, &tree_error),
                           try_mult (tokens, n_tok, error), &tree_error);
        if (tree_error)
        {
            tree_print_error (tree_error);
            *error = PARSE_ERROR_TREE;
            return NULL;
        }
    }
    else
    {
        val = try_mult (tokens, n_tok, error);
    }
    while (CUR_TYPE == LEX_TYPE_OPER &&
          (CUR_OPER == LEX_OPER_ADD || CUR_OPER == LEX_OPER_SUB))
    {
        LexOperator oper = CUR_OPER;
        (*n_tok)++;

        if (couple_oper (tokens, n_tok) == true)// TODO: переименовать функцию
        {
            *error = PARSE_ERROR_SYNTAX;
            fprintf (stderr, "Введено некорректное выражение: оператор \"%s\" сразу после оператора \"%s\"."
                             "Работа программы завершена досрочно :(\n", OPERS[(int) CUR_OPER].name,
                                                                         OPERS[(int) PREV_OPER].name);
            tree_dtor (val); // TODO: вспомнить, для чего это
            return NULL;
        }

        Node* val2 = try_mult (tokens, n_tok, error);

        TreeError tree_error = TREE_ERROR_OK; // TODO: спросить, насколько норм так обрабатывать ошибки и норм ли забивать в некоторых случаях (когда в return)
        if (oper == LEX_OPER_ADD)
            val = create_node (LEX_TYPE_OPER, LEX_OPER_ADD, val, val2, &tree_error);
        else
            val = create_node (LEX_TYPE_OPER, LEX_OPER_SUB, val, val2, &tree_error);

        if (tree_error)
        {
            tree_print_error (tree_error);
            *error = PARSE_ERROR_TREE;
            return NULL;
        }
    }
    return val;
}

Node* try_pow (const Darray* tokens, size_t* n_tok, ParseError* error)
{
    Node* val = try_parenthesis (tokens, n_tok, error);
    if (CUR_TYPE == LEX_TYPE_OPER && CUR_OPER == LEX_OPER_POW)
    {
        (*n_tok)++;

        if (couple_oper (tokens, n_tok) == true)
        {
            *error = PARSE_ERROR_SYNTAX;
            fprintf (stderr, "Введено некорректное выражение: оператор \"%s\" сразу после оператора \"%s\"."
                             "Работа программы завершена досрочно :(\n", OPERS[(int) CUR_OPER].name,
                                                                         OPERS[(int) PREV_OPER].name);
            tree_dtor (val);
            return NULL;
        }

        Node* val2 = try_parenthesis (tokens, n_tok, error);

        TreeError tree_error = TREE_ERROR_OK;
        val = create_node (LEX_TYPE_OPER, LEX_OPER_POW, val, val2, &tree_error);

        if (tree_error)
        {
            tree_print_error (tree_error);
            *error = PARSE_ERROR_TREE;
            return NULL;
        }

        if (CUR_TYPE == LEX_TYPE_OPER && CUR_OPER == LEX_OPER_POW)
        {
            *error = PARSE_ERROR_SYNTAX;
            fprintf (stderr, "Введено некорректное выражение: "
                             "используйте скобки для записи.\n");
            return NULL;
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
            fprintf (stderr, "5Введено некорректное выражение: оператор \"%s\" сразу после оператора \"%s\"."
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
        {
            *error = PARSE_ERROR_SYNTAX;
            return NULL;
        }

        (*n_tok)++;

        TreeError tree_error = TREE_ERROR_OK;
        switch (OPERS[n_oper].op_enum)
        {
            case LEX_OPER_SIN:
                val = create_node (LEX_TYPE_OPER, LEX_OPER_SIN,  NULL, val, &tree_error);
                return val;
            case LEX_OPER_COS:
                val = create_node (LEX_TYPE_OPER, LEX_OPER_COS,  NULL, val, &tree_error);
                return val;
            case LEX_OPER_LN:
                val = create_node (LEX_TYPE_OPER, LEX_OPER_LN,   NULL, val, &tree_error);
                return val;
            case LEX_OPER_SQRT:
                val = create_node (LEX_TYPE_OPER, LEX_OPER_SQRT, NULL, val, &tree_error);
                return val;
            case LEX_OPER_EXP:
                val = create_node (LEX_TYPE_OPER, LEX_OPER_EXP,  NULL, val, &tree_error);
                return val;
            case LEX_OPER_ADD:
            case LEX_OPER_SUB:
            case LEX_OPER_MUL:
            case LEX_OPER_DIV:
            case LEX_OPER_POW:
            case LEX_OPER_ASSIGN:
            case LEX_OPER_NONE:
            default:
                *error = PARSE_ERROR_SYNTAX;
                return NULL;
        }

        if (tree_error)
        {
            tree_print_error (tree_error);
            *error = PARSE_ERROR_TREE;
            return NULL;
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
        {
            *error = PARSE_ERROR_SYNTAX;
            return NULL;
        }

        (*n_tok)++;

        return val;
    }

    return try_item (tokens, n_tok, error);
}

Node* try_mult (const Darray* tokens, size_t* n_tok, ParseError* error)
{
    Node* val = try_func (tokens, n_tok, error);
    while (CUR_TYPE == LEX_TYPE_OPER &&
          (CUR_OPER == LEX_OPER_MUL || CUR_OPER == LEX_OPER_DIV))
    {
        LexOperator oper = CUR_OPER;
        (*n_tok)++;

        if (couple_oper (tokens, n_tok) == true)
        {
            *error = PARSE_ERROR_SYNTAX;
            fprintf (stderr, "Введено некорректное выражение: оператор \"%s\" сразу после оператора \"%s\"."
                             "Работа программы завершена досрочно :(\n", OPERS[(int) CUR_OPER].name,
                                                                         OPERS[(int) PREV_OPER].name);
            tree_dtor (val); // TODO: вспомнить для чего
            return NULL;
        }
        Node* val2 = try_func (tokens, n_tok, error);

        TreeError tree_error = TREE_ERROR_OK;
        if (oper == LEX_OPER_MUL) // TODO: здесь падает
            val = create_node (LEX_TYPE_OPER, LEX_OPER_MUL, val, val2, &tree_error);
        else
            val = create_node (LEX_TYPE_OPER, LEX_OPER_DIV, val, val2, &tree_error);

        if (tree_error)
        {
            tree_print_error (tree_error);
            *error = PARSE_ERROR_TREE;
            return NULL;
        }
    }
    return val;
}

Node* try_item (const Darray* tokens, size_t* n_tok, ParseError* error)
{
    Node* new_node = NULL;
    double val = 0;
    size_t n_var = SIZE_MAX;
    TreeError tree_error = TREE_ERROR_OK;

    if (CUR_TYPE == LEX_TYPE_NUM)
    {
        val = CUR_NUMBER;
        (*n_tok)++;
        tree_error = TREE_ERROR_OK;
        new_node = create_node (LEX_TYPE_NUM, val, NULL, NULL, &tree_error);
    }
    else if (CUR_TYPE == LEX_TYPE_VAR)
    {
        n_var = VAR_NUMBER;
        (*n_tok)++;
        tree_error = TREE_ERROR_OK;
        new_node = create_node (LEX_TYPE_VAR, (double) n_var, NULL, NULL, &tree_error);
    }
    else
    {
        *error = PARSE_ERROR_SYNTAX;
        return NULL;
    }

    if (tree_error)
    {
        tree_print_error (tree_error);
        *error = PARSE_ERROR_TREE;
        return NULL;
    }
    return new_node;
}

bool couple_oper (const Darray* tokens, size_t* n_tok)
{
    if (CUR_TYPE == LEX_TYPE_OPER)
        if (!OPERS[(int) CUR_OPER].is_func)
            return true;

    return false;
}

ParseError write_tree_in_file (const Node* root, char* text_tree, FILE* output_file) // TODO: распечатку вынести в фронтенд
{
    size_t tree_text_size = 0;
    ParseError error = get_text_tree (root, text_tree, &tree_text_size);
    if (error)
    {
        parse_print_error (error);
        return error;
    }

    printf ("TEXT:: %s\n", text_tree);

    size_t num_written = fwrite (text_tree, sizeof (char), tree_text_size, output_file);
    if (num_written != tree_text_size)
        return PARSE_ERROR_FWRITE;

    return PARSE_ERROR_OK;
}

ParseError get_text_tree (const Node* node, char* text_tree, size_t* text_size)
{
    if (node != NULL)
    {
        LexType node_type = node->elem.type;

        switch (node_type)
        {
            case LEX_TYPE_OPER:
            {
                int oper = (int) node->elem.elem.oper;
                int length = 0;
                sprintf (text_tree + *text_size, "#%d#%d%n", node_type, oper, &length);
                (*text_size) += length;
                break;
            }
            case LEX_TYPE_NUM:
            {
                double num = node->elem.elem.num;
                int length = 0;
                sprintf (text_tree + *text_size, "#%d#%g%n", node_type, num, &length);
                (*text_size) += length;
                break;
            }
            case LEX_TYPE_VAR:
            {
                int var_number = (int) node->elem.elem.var_number;
                int length = 0;
                sprintf (text_tree + *text_size, "#%d#%d%n", node_type, var_number, &length);
                (*text_size) += length;
                break;
            }
            case LEX_TYPE_DELIM:
            {
                int delim = (int) node->elem.elem.delim;
                int length = 0;
                sprintf (text_tree + *text_size, "#%d#%d%n", node_type, delim, &length);
                (*text_size) += length;
                break;
            }
            case LEX_TYPE_TXT:
            default:
                return PARSE_ERROR_SYNTAX;
        }

        ParseError error = get_text_tree (node->left, text_tree, text_size);
        if (error)
        {
            parse_print_error (error);
            return error;
        }

        error = get_text_tree (node->right, text_tree, text_size); // TODO: подумать, как убрать копипаст
        if (error)
        {
            parse_print_error (error);
            return error;
        } // TODO: обернуть в макрос
    }
    else
    {
        text_tree[(*text_size)++] = '_';
    }

    return PARSE_ERROR_OK;
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
        case PARSE_ERROR_FWRITE:
            return "Parse: Ошибка fwrite.";
        default:
            return "Parse: Нужной ошибки не найдено...";
    }
}
