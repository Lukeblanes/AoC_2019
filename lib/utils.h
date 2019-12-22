#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// returns 1 if c is in delimiters
uint32_t check_delimiters(const char c, const char* delimiters)
{
    uint32_t charPos = 0; 
    char ch = delimiters[charPos]; 
    while(ch != '\0')
    {
        if ( ch == c) {
            return 1;
        }
        ch = delimiters[++charPos];
    }
    return 0;
}

char** tokenizer_with_delimiters(const char* line, const char* delimiters, uint32_t* token_count)
{
    // If we want to tokenize without passing back the token_count we can pass NULL
    if(token_count == NULL) 
    {
        uint32_t a = 0;
        token_count = &a;
    }
    else // we set token_count to 0
        (*token_count) = 0;
    char ch = line[0]; 

    uint32_t charPos = 0;
    uint32_t previousIsDelim = 0; // to skip counting when 

    // First pass where we count tokens
    while(ch != '\0')
    {
        if ( check_delimiters(ch, delimiters) == 1) {
            if(previousIsDelim == 0)
                (*token_count)++;
            previousIsDelim = 1;
        }
        else
        {
            previousIsDelim = 0;
        }
        
        ch = line[++charPos];
    }
    (*token_count)++; // one last count. We assume that a delimiter will never be a last char

    // Second pass where we create and initialize the token array
    char** tokens = malloc(sizeof(char*) * (*token_count));
//    const char delimiter_string[2] = {delimiter, '\0'};

    // we copy the original line to not modify the original line
    char* token = malloc(sizeof(char) * strlen(line) + 1);
    char* deleter = token; // to delete since token will stop pointing to start
    strcpy(token, line);

    // first call of strtok needs char* to be passed
    token = strtok(token, delimiters); 

    uint32_t counter = 0;
    while(token != NULL)
    {
        if(counter == (*token_count))
        {
            printf("Inserting more tokens than counted.\n");
            exit(-1);
        }
        tokens[counter] = malloc(sizeof(char) * strlen(token) + 1);
        strcpy(tokens[counter], token);
        
        token = strtok(NULL, delimiters);
        counter++;
    }
    free(deleter);
    return tokens;
}

char** tokenizer_with_delimiter(const char* line, const char delimiter, uint32_t* token_count)
{
    // If we want to tokenize without passing back the token_count we can pass NULL
    if(token_count == NULL) 
    {
        uint32_t a = 0;
        token_count = &a;
    }
    char ch = line[0]; 

    uint32_t charPos = 0;
    // First pass where we count tokens
    while(ch != '\0')
    {
        if ( ch == delimiter) {
            (*token_count)++;
        }
        ch = line[++charPos];
    }
    (*token_count)++; // one last count. We assume that a delimiter will never be a last char

    // Second pass where we create and initialize the token array
    char** tokens = malloc(sizeof(char*) * (*token_count));
    const char delimiter_string[2] = {delimiter, '\0'};

    // we copy the original line to not modify the original line
    char* token = malloc(sizeof(char) * strlen(line) + 1);
    char* deleter = token; // to delete since token will stop pointing to start
    strcpy(token, line);

    // first call of strtok needs char* to be passed
    token = strtok(token, delimiter_string); 

    uint32_t counter = 0;
    while(token != NULL)
    {
        if(counter == (*token_count))
        {
            printf("Inserting more tokens than counted.\n");
            exit(-1);
        }
        tokens[counter] = malloc(sizeof(char) * strlen(token) + 1);
        strcpy(tokens[counter], token);
        
        token = strtok(NULL, delimiter_string);
        counter++;
    }
    free(deleter);
    return tokens;
}

void tokenizer_print_tokens(char** tokens, int tokenCount)
{
    printf("There are %d tokens and are: \n", tokenCount);
    for (int i = 0; i < tokenCount; ++i)
    {
        printf("token %d:%s\n", i, tokens[i]);
    }
}

void free_char_pp(char** stringArray, int arraySize)
{
    for(uint16_t i = 0; i < arraySize; i++)
        free(stringArray[i]);
    free(stringArray);
}

void cmd_input_validate(int argc, char** argv, const char* format, uint32_t meant_argc)
{
    if(argc != meant_argc)
    {
        printf("Error in input. Format should be: %s %s\n", argv[0], format);
        exit(-1);
    }
}

void grid_print(char **grid, uint32_t width, uint32_t height)
{
    for (uint32_t i = 0; i < height; i++) {
        for (uint32_t j = 0; j < width; j++)
            printf("%c", grid[i][j]);
        printf("\n");
    }
}