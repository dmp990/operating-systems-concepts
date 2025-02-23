#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_LINE 80
#define MAX_ARGS MAX_LINE / 2 + 1

int prompt_and_read_command(char *buffer)
{
    printf("shell2>");
    fflush(stdout);

    char *line;
    size_t count = 0;
    int return_val = getline(&line, &count, stdin);
    if (return_val == -1)
    {
        perror("Error reading input\n");
        exit(1);
    }
    // Success
    strcpy(buffer, line);
    return return_val;
}

int tokenize(char *buffer, char *args[MAX_ARGS])
{
    char *token = strtok(buffer, " ");
    int index = 0;

    while (token != NULL && index < MAX_ARGS)
    {
        args[index++] = token;
        token = strtok(NULL, " ");
    }
    return index;
}

int main(void)
{
    char *args[MAX_ARGS]; /* command line arguments */
    int should_run = 1;   /* flag to determine when to exit program */
    char buffer[MAX_LINE];
    char *recent = NULL;

    while (should_run)
    {
        prompt_and_read_command(buffer); // Read user input into buffer

        if (strcmp(buffer, "!!\n") == 0)
        {
            // Recent requested
            if (recent == NULL)
            {
                printf("No commands in history\n");
                continue;
            }
            strcpy(buffer, recent);
            printf("%s\n", buffer);
        }

        // Copy buffer to recent
        if (recent != NULL)
        {
            free(recent);
        }
        recent = malloc(sizeof(buffer) + 1);
        strcpy(recent, buffer);

        int count = tokenize(buffer, args); // Tokenize buffer and store tokens in args

        printf("Tokens:\n");
        for (int i = 0; i < count; i++)
        {
            printf("%s\n", args[i]);
        }

        pid_t pid = fork();
        if (pid < 0)
        {
            perror("Fork error\n");
            exit(1);
        }
        else if (pid > 0)
        {
            /* Parent Process */
            if (strcmp(args[count - 1], "&") != 0)
            {
                printf("Waiting\n");
                waitpid(pid, NULL, 0);
            }
            printf("here\n");
        }
        else {
            /* Child Process */
            if (execvp(args[0], args) == -1)
            {
                perror("execvp failed\n");
                exit(1);
            }
        }

    }
    return 0;
}
