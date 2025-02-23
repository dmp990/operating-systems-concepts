#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "helpers.h"


int main(int argc, char *argv[])
{
    int has_recent = 0;
    int should_run = 1;

    char *most_recent_command[MAX_TOKENS] = malloc(MAX_TOKENS * sizeof(char *));
    if (!most_recent_command) {
        perror("Failed to allocate memory");
        exit(1);
    }

    while (should_run)
    {
        int recent_requested = 0;
        int should_run_in_background = 0;

        printf("shell>");
        fflush(stdout);

        char command[MAX_LINE];
        fgets(command, sizeof(command), stdin);

        // Tokenize user input
        int count = 0;
        char **tokens = split_by_whitespace(command, &count);


        if (strcmp(tokens[0], "!!") == 0)
        {
            // User requested most recent command
            if (!has_recent)
            {
                perror("No commands in history\n");
                free_tokens(tokens);
                continue;
            }
            recent_requested = 1;
        }

        char *last_token = tokens[count - 1];

        if (strcmp(last_token, "&") == 0)
        {
            should_run_in_background = 1;
            // Remove last token, replace with NULL for now
            tokens[count - 1] = NULL;
        }

        if (!recent_requested)
        {
            // Store current as recent
            for (int i = 0; i < MAX_TOKENS; i++) {
                free(most_recent_command[i]);
                most_recent_command[i] = NULL;
            }

            for (int i = 0; i < count; i++)
            {
                most_recent_command[i] = strdup(tokens[i]);
            }
            has_recent = 1;
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
            if (recent_requested)
            {
                free_tokens(tokens);
                if (execvp(most_recent_command[0], most_recent_command) == -1)
                {
                    perror("execvp failed\n");
                    exit(1);
                }
            }
            else
            {
                if (execvp(tokens[0], tokens) == -1)
                {
                    perror("execvp failed\n");
                    exit(1);
                }
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
