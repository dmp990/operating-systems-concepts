#include "helpers.h"

void print_char_pts_arr(char *tokens[])
{
    for (int i = 0; i < MAX_ARGS_LENGTH; i++)
    {
        printf("tokens[%d]: %p\n", i, (void *) tokens[i]);
    }
}

int main(void)
{
    char command[MAX_COMMAND_LENGTH];
    char *history = NULL;
    char *tokens[MAX_ARGS_LENGTH] = {NULL};
    char delimiters[] = " \n";

    int should_run = 1;

    while (should_run)
    {
        // Get User Input
        if (get_user_input(command) == 1)
        {
            printf("\nExiting shell (EOF received)\n");
            break; // Exit on EOF (e.g., Ctrl+D)
        }

        // printf("Entered: $%s^\n", command);

        // User requested previous command
        if (strcmp(command, "!!") == 0)
        {
            if (!history)
            {
                printf("No history\n");
                continue;
            }
            printf("%s\n", history);
            strcpy(command, history);
        }
        else
        {
            if (!history)
            {
                history = malloc(sizeof(char) * MAX_COMMAND_LENGTH);
            }
            strcpy(history, command);
        }

        int num_tokens = 0;
        if (tokenize(command, tokens, delimiters, &num_tokens) == 1)
        {
            continue; // No tokens found
        }

        // User requested to exit
        if (strcmp(tokens[0], "exit") == 0)
        {
            printf("Exiting\n");
            should_run = 0;
        }

        if (should_run)
        {
            execute_command(tokens, num_tokens);
        }

        // Free allocated memory for tokens after use
        for (int i = 0; i < num_tokens; i++)
        {
            free(tokens[i]);
            tokens[i] = NULL;
        }
    }

    // Free allocated memory for history after use
    if (history)
    {
        free(history);
    }
    return 0;
}
