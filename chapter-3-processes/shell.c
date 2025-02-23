#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <ctype.h>
#include <unistd.h>


#define MAX_TOKENS 100  // Maximum number of tokens

// Function to split a string by whitespace
char **split_by_whitespace(const char *input, int *count) {
    if (input == NULL) {
        return NULL;
    }

    // Allocate space for tokens
    char **tokens = malloc(MAX_TOKENS * sizeof(char *));
    if (!tokens) {
        perror("Failed to allocate memory");
        exit(EXIT_FAILURE);
    }

    // Make a copy of the input string to avoid modifying the original
    char *copy = strdup(input);
    if (!copy) {
        perror("Failed to allocate memory");
        free(tokens);
        exit(EXIT_FAILURE);
    }

    char *token = strtok(copy, " \t\n"); // Tokenize using spaces, tabs, and newlines
    int index = 0;

    while (token != NULL && index < MAX_TOKENS - 1) {
        tokens[index] = strdup(token);  // Copy the token
        if (!tokens[index]) {
            perror("Failed to allocate memory");
            exit(EXIT_FAILURE);
        }
        index++;
        token = strtok(NULL, " \t\n"); // Get the next token
    }

    tokens[index] = NULL; // Null-terminate the array
    *count = index;  // Store the number of tokens

    free(copy); // Free the temporary copy (not the tokenized parts)
    return tokens;
}

// Function to free the allocated tokens
void free_tokens(char **tokens) {
    if (!tokens) return;
    for (int i = 0; tokens[i] != NULL; i++) {
        free(tokens[i]);
    }
    free(tokens);
}

int main(int argc, char *argv[])
{
    int should_run = 1;
    while (should_run)
    {
        printf("shell>");
        fflush(stdout);

        char command[100];
        fgets(command, sizeof(command), stdin);

        int count = 0;
        char **tokens = split_by_whitespace(command, &count);

        /**
* After reading user input, the steps are:
* (1) fork a child process using fork()
* (2) the child process will invoke execvp()
* (3) parent will invoke wait() unless command included &
*/
        pid_t pid = fork();
        if (pid == 0)
        {
            // Child Process
            if (execvp(tokens[0], tokens) == -1)
            {
                perror("execvp failed\n");
            }
        }
        else if (pid > 0)
        {
            printf("Parent process. Wating for child with pid %d to end\n", pid);
            wait(NULL);
        } else {
            // Error
            printf("Error occurred\n");
        }


        // Free allocated memory
        free_tokens(tokens);
    }
    return 0;
}
