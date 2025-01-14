#include "../../../front_end/include/core/lexer.hh" // TODO: НАСТРОИТЬ ПУТИ В CMAKE, !ЗАТЕМ!сделать общий cmake для всего проекта

void asm_add (FILE* output_file)
{
    fprintf(output_file, "add\n");
}

void asm_sub (FILE* output_file)
{
    fprintf(output_file, "sub\n");
}

void asm_mul (FILE* output_file);
{
    fprintf(output_file, "mul\n");
}

void asm_div (FILE* output_file)
{
    fprintf(output_file, "div\n");
}

void asm_pow (FILE* output_file)
{
    fprintf(output_file, "pow\n");
}

void asm_assign (FILE* output_file)
{
    fprintf(output_file, "pop rax\n"
                         "pop [rax]\n");
}

void asm_sin (FILE* output_file)
{
    fprintf(output_file, "sin\n");
}

void asm_cos (FILE* output_file)
{
    fprintf(output_file, "cos\n");
}

void asm_ln (FILE* output_file)
{
    fprintf(output_file, "ln\n");
}

void asm_sqrt (FILE* output_file)
{
    fprintf(output_file, "sqrt\n");
}

void asm_exp (FILE* output_file)
{
    fprintf(output_file, "exp\n");
}
