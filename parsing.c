#include "mpc.h"
#include <stdio.h>
#include <stdlib.h>

/* if we are compiling on Windows compile these functions */
#ifdef _WIN32
#include <string.h>

/* declare a buffer for user input of size 2048 */
static char buffer[2048];

/* fake readline function */
char* readline(char* prompt){
  fputs(prompt, stdout);
  fgets(buffer, 2048, stdin);
  char* cpy = malloc(strlen(buffer) + 1);
  strcpy(cpy, buffer);
  cpy[strlen(cpy) -1] = '\0';
  return cpy;
}

/* fake add_history function */
void add_history(char* unused){}

/* otherwise include the editline headers */
#else
#include <editline/readline.h>
#include <editline/history.h>
#endif

int main(int argc, char** argv){

  /* grammar for the language of polish notation */

  /* create some parsers */
  mpc_parser_t* Number   = mpc_new("number");
  mpc_parser_t* Operator = mpc_new("operator");
  mpc_parser_t* Expr     = mpc_new("expr");
  mpc_parser_t* Lispy    = mpc_new("lispy");

  /* define parsers with the following language */
  mpca_lang(MPCA_LANG_DEFAULT,
	    "                                                    \
               number  : /-?[0-9]+/ ;                            \
               operator: '+' | '-' | '*' | '/' ;                 \
               expr    : <number> | '(' <operator> <expr>+ ')' ; \
               lispy   : /^/ <operator> <expr>+ /$/ ;            \
            ",
	    Number, Operator, Lispy);

  /* print version and exit information */
  puts("Lispy Version 0.0.0.0.1");
  puts("Press Ctrl+c to Exit\n");

  /* never ending loop */
  while(1){

    /* now in either case readline will be correctly defined */
    char* input = readline("lispy> ");
    add_history(input);

    printf("No you're a %s\n", input);
    free(input);

  }

  /* undefine and delete parsers */
  mpc_cleanup(4, Number, Operator, Expr, Lispy);
  return 0;

}




