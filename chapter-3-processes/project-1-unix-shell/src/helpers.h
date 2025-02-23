#ifndef HELPERS_H
#define HELPERS_H

#define MAX_LINE 80 /* The maximum length command */
#define MAX_TOKENS MAX_LINE / 2 + 1  // Maximum number of tokens

// prototypes
char **split_by_whitespace(const char *input, int *count);
void free_tokens(char **tokens);

#endif // HELPERS_H
