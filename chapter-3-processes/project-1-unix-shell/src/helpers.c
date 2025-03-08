#include "helpers.h"

int get_user_input(char *command)
{
    printf("shell> ");
    fflush(stdout);
    char *rd = fgets(command, MAX_COMMAND_LENGTH, stdin);

    if (rd)
    {
        int length = strlen(command);
        if (length > 0 && command[length - 1] == '\n')
        {
            command[length - 1] = '\0'; // Remove trailing new line character
        }
        return 0;
    }
    return 1;
}

int tokenize(char *command, char *tokens[], char *delimiters, int *num_tokens)
{
    int i = *num_tokens = 0;

    char *token = strtok(command, delimiters);
    if (token == NULL)
    {
        return 1;
    }

    while (token)
    {
        if (tokens[i])
        {
            free(tokens[i]);
        }
        tokens[i] = (char *)malloc(sizeof(char) * (strlen(token) + 1));
        strcpy(tokens[i++], token);
        token = strtok(NULL, delimiters);
    }
    tokens[i] = NULL;
    *num_tokens = i;

    return 0;
}

void execute_command(char *tokens[], int num_tokens)
{
    if (num_tokens == 0)
    {
        return;
    }

    int background = 0;
    if (strcmp(tokens[num_tokens - 1], "&") == 0)
    {
        background = 1;
        tokens[num_tokens - 1] = NULL;
        num_tokens--;
    }

    // Assume either one of < or > or | or none of these will be present in a command
    int input_redirection = -1, output_redirection = -1;

    char filename[100];

    for (int i = 0; i < num_tokens; i++)
    {
        if (strcmp(tokens[i], "<") == 0 && i + 1 < num_tokens)
        {
            input_redirection = i;
            strcpy(filename, tokens[i + 1]);
        }
        if (strcmp(tokens[i], ">") == 0 && i + 1 < num_tokens)
        {
            output_redirection = i;
            strcpy(filename, tokens[i + 1]);
        }
    }

    int original_stdin = dup(STDIN_FILENO);
    int original_stdout = dup(STDOUT_FILENO);

    if (input_redirection != -1)
    {
        /*
        int dup2(int oldfd, int newfd);
      The dup() system call allocates a new file descriptor that refers
      to the same open file description as the descriptor oldfd
      The dup2() system call performs the same task as dup(), but
      instead of using the lowest-numbered unused file descriptor, it
      uses the file descriptor number specified in newfd
      */
        int fd = open(filename, O_RDONLY);
        dup2(fd, STDIN_FILENO);
        close(fd);

        tokens[input_redirection] = NULL;
    }
    if (output_redirection != -1)
    {
        int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        dup2(fd, STDOUT_FILENO);
        close(fd);

        tokens[output_redirection] = NULL;
    }

    pid_t pid = fork();

    if (pid > 0)
    {
        // Parent process
        if (!background)
        {
            waitpid(pid, NULL, 0);
        }
    }
    else if (pid == 0)
    {
        // Child process
        execvp(tokens[0], tokens);
        perror("execvp");
        exit(1);
    }
    else
    {
        // Error forking
        printf("Error: Could not fork.\n");
    }

    dup2(original_stdin, STDIN_FILENO);
    dup2(original_stdout, STDOUT_FILENO);

    close(original_stdin);
    close(original_stdout);
}
