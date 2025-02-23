#include <ctype.h>
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

#include "helpers.h"

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
            // Parent process
            printf("Parent process. Wating for child with pid %d to end\n", pid);
            wait(NULL);
        }
        else
        {
            // Error
            printf("Error occurred\n");
        }

        // Free allocated memory
        free_tokens(tokens);
    }
    return 0;
}
