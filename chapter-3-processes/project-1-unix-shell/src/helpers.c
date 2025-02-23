#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "helpers.h"

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
