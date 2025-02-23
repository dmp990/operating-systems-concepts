#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "helpers.h"

int main(int argc, char *argv[])
{
    int should_run = 1;
    while (should_run)
    {
        int should_run_in_background = 0;
        printf("shell>");
        fflush(stdout);

        char command[100];
        fgets(command, sizeof(command), stdin);

        int count = 0;
        char **tokens = split_by_whitespace(command, &count);

        char *last_token = tokens[count - 1];

        if (strcmp(last_token, "&") == 0)
        {
            should_run_in_background = 1;
            // Remove last token, replace with NULL for now
            tokens[count - 1] = NULL;
        }

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
                exit(1);
            }
        }
        else if (pid > 0)
        {
            // Parent process
            // printf("Parent process. Wating for child with pid %d to end\n", pid);
            fflush(stdout);
            if (!should_run_in_background)
            {
                printf("Waiting\n");
                fflush(stdout);
                waitpid(pid, NULL, 0);
            }

            printf("I am here\n");
            fflush(stdout);
        }
        else
        {
            // Error
            perror("Fork failed\n");
        }

        // Free allocated memory
        free_tokens(tokens);
    }
    return 0;
}
