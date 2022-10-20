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
