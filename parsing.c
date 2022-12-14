#include <stdio.h>
#include <stdlib.h>
#include "mpc.h"

/* If we are compiling on Windows compile these functions */
#ifdef _WIN32
// #include <string.h>

// static char buffer[2048];

// /* Fake readline function */
// char *readline(char *prompt)
// {
//     fputs(prompt, stdout);
//     fgets(buffer, 2048, stdin);
//     char *cpy = malloc(strlen(buffer) + 1);
//     strcpy(cpy, buffer);
//     cpy[strlen(cpy) - 1] = '\0';
//     return cpy;
// }

// /* Fake add_history function */
// void add_history(char *unsued) {}
#include "wineditline-2.206/include/editline/readline.h"

/* Otherwise include the editline headers */
#else
#include <editline/readline.h>
#include <editline/history.h>
#endif

long eval(mpc_ast_t *t);
long eval_op(long x, char *op, long y);
void print_node(mpc_ast_t *node);

int main(int argc, char **argv)
{

    /* Create some parsers */
    mpc_parser_t *Number = mpc_new("number");
    mpc_parser_t *Operator = mpc_new("operator");
    mpc_parser_t *Expr = mpc_new("expr");
    mpc_parser_t *Lispy = mpc_new("lispy");

    /* Define them with the following language */
    mpca_lang(MPCA_LANG_DEFAULT,
              " \
    number: /-?[0-9]+/ ; \
    operator: '+' | '-' | '*' | '/' ; \
    expr: <number> | '(' <operator> <expr>+ ')' ; \
    lispy: /^/ <operator> <expr>+ /$/ ; \
    ",
              Number, Operator, Expr, Lispy);

    /* Print Version and Exit Information */
    puts("Lispy Version 0.0.0.0.1");
    puts("Press Ctrl+c to Exit\n");

    /* In a never ending loop */
    while (1)
    {

        /* Output our prompt */
        char *input = readline("lispy> ");

        /* Add input to hisory */
        add_history(input);

        mpc_result_t r;
        if (mpc_parse("<stdin>", input, Lispy, &r))
        {
            /* On success print the AST */
            mpc_ast_print(r.output);
            // print_node(r.output);
            long result = eval(r.output);
            printf("%li\n", result);
            mpc_ast_delete(r.output);
        }
        else
        {
            /* Otherwise print the error */
            mpc_err_print(r.error);
            mpc_err_delete(r.error);
        }

        /* Free retrieved input */
        free(input);
    }

    /* Undefine and delete our parsers */
    mpc_cleanup(4, Number, Operator, Expr, Lispy);

    return 0;
}

long eval(mpc_ast_t *t)
{
    print_node(t);
    /* If tagged as number return it directly. */
    if (strstr(t->tag, "number"))
    {
        return atoi(t->contents);
    }

    /* The operator is always second child. */
    char *op = t->children[1]->contents;
    print_node(t->children[1]);
    /* We store the third child in 'x' */
    long x = eval(t->children[2]);

    /* Iterate the remaining children and combining. */
    int i = 3;
    while (strstr(t->children[i]->tag, "expr"))
    {
        x = eval_op(x, op, eval(t->children[i]));
        i++;
    }
    return x;
}

/* Use operator string to see which operation to perform */
long eval_op(long x, char *op, long y)
{
    if (strcmp(op, "+") == 0)
    {
        return x + y;
    }
    if (strcmp(op, "-") == 0)
    {
        return x - y;
    }
    if (strcmp(op, "*") == 0)
    {
        return x * y;
    }
    if (strcmp(op, "/") == 0)
    {
        return x / y;
    }
    return 0;
}

void print_node(mpc_ast_t *node)
{
    printf("Tag: %s\n", node->tag);
    printf("Contents: %s\n", node->contents);
    printf("Number of children: %i\n", node->children_num);
}