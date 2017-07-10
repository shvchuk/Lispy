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

  return 0;

}




