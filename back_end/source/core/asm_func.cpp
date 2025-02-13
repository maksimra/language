#include <stdio.h>
#include "lexer.hpp"

#define PRINT_ASM(str) fprintf(output_file, str)

void asm_add (FILE* output_file)
{
    PRINT_ASM("add\n");
}

void asm_sub (FILE* output_file)
{
    PRINT_ASM("sub\n");
}

void asm_mul (FILE* output_file)
{
    PRINT_ASM("mul\n");
}

void asm_div (FILE* output_file)
{
    PRINT_ASM("div\n");
}

void asm_pow (FILE* output_file)
{
    PRINT_ASM("pow\n");
}

void asm_assign (FILE* output_file)
{
    PRINT_ASM("pop rax\n"
              "pop rbx\n"
              "push rax\n"
              "pop [rbx]\n");
}

void asm_sin (FILE* output_file)
{
    PRINT_ASM("sin\n");
}

void asm_cos (FILE* output_file)
{
    PRINT_ASM("cos\n");
}

void asm_ln (FILE* output_file)
{
    PRINT_ASM("ln\n");
}

void asm_sqrt (FILE* output_file)
{
    PRINT_ASM("sqrt\n");
}

void asm_exp (FILE* output_file)
{
    PRINT_ASM("exp\n");
}
