#ifndef helpers_h
#define helpers_h

#define MAX_COMMAND_LENGTH 100                       // Maximum length of command
#define MAX_ARGS_LENGTH (MAX_COMMAND_LENGTH / 2 + 1) // Maximum number of args

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

// Prototypes
int get_user_input(char *command);
int tokenize(char *command, char *tokens[], char *delimiters, int *num_tokens);
void execute_command(char *tokens[], int num_tokens);

#endif // helpers_h
