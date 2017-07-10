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

/* create enumeration of possible error types */
enum { LERR_DIV_ZERO, LERR_BAD_OP, LERR_BAD_NUM };

/* create enumeration of possible lval types. lval - Lisp Value */
enum { LVAL_NUM, LVAL_ERR };

/* declare new lval Struct */
typedef struct {
  int type;
  long num;
  int err;
} lval;

/* create a new number type lval */
lval lval_num(long x){
  lval v;
  v.type = LVAL_NUM;
  v.num = x;
  return v;
}

/* create a new error type lval */
lval lval_err(int x){
  lval v;
  v.type = LVAL_ERR;
  v.err = x;
  return v;
}

/* print an "lval" */
void lval_print(lval v){
  switch (v.type){
    /* in the case the type is a number print it */
    /* then 'break' out of the switch */
  case LVAL_NUM: printf("%li", v.num); break;

    /* in the case the type is an error */
  case LVAL_ERR:
    /*check what type of error it is and print it */
    if(v.err == LERR_DIV_ZERO){
      printf("Error: Division By Zero!");
    }
    if(v.err == LERR_BAD_OP) {
      printf("Error: Invalid Operator!");
    }
    if(v.err == LERR_BAD_NUM) {
      printf("Error: Invalid Number!");
    }
    break;
  }
}

/* print an "lval" followed by a new line */
void lval_println(lval v){ lval_print(v); putchar('\n'); }

/* use operator string to see which operation to perform */
lval eval_op(lval x, char* op, lval y){

  /* if either value is an error return it */
  if(x.type == LVAL_ERR) {return x; }
  if(y.type == LVAL_ERR) {return y; }

  /* otherwise do maths on the number values */
  if (strcmp(op, "+") == 0) {return lval_num(x.num + y.num); }
  if (strcmp(op, "-") == 0) {return lval_num(x.num - y.num); }
  if (strcmp(op, "*") == 0) {return lval_num(x.num * y.num); }
  if (strcmp(op, "/") == 0) {
    /* if second operand is zero return error */
    return y.num == 0 ? lval_err(LERR_DIV_ZERO) : lval_num(x.num / y.num);
  }

  return lval_err(LERR_BAD_OP);
}

lval eval(mpc_ast_t* t){

  if (strstr(t->tag, "number")){

    /* check if there is some error in conversion */
    errno = 0;
    long x = strtol(t->contents, NULL, 10);
    return errno != ERANGE ? lval_num(x) : lval_err(LERR_BAD_NUM);
  }

  /* the operator is always second child */
  char* op = t->children[1]->contents;

  /* we store the third child in 'x' */
  lval x = eval(t->children[2]);

  /* iterate the remaining children and combining */
  int i = 3;
  while (strstr(t->children[i]->tag, "expr")) {
    x = eval_op(x, op, eval(t->children[i]));
    i++;
  }

  return x;
}

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
	    Number, Operator, Expr, Lispy);

  /* print version and exit information */
  puts("Lispy Version 0.0.0.0.1");
  puts("Press Ctrl+c to Exit\n");

  /* never ending loop */
  while(1){

    /* now in either case readline will be correctly defined */
    char* input = readline("lispy> ");
    add_history(input);

    /* attempt to parse the user info */
    mpc_result_t r;
    if(mpc_parse("<stdin>", input, Lispy, &r)){

      lval result = eval(r.output);
      lval_println(result);
      mpc_ast_delete(r.output);
      
    } else {
      /* otherwise print the error */
      mpc_err_print(r.error);
      mpc_err_delete(r.error);
    }

    free(input);

  }

  /* undefine and delete parsers */
  mpc_cleanup(4, Number, Operator, Expr, Lispy);
  
  return 0;
}









