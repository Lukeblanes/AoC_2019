#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

uint32_t tokenizer_get_splitter_count(const char* line, const char splitter)
{
    uint32_t counter = 0;
    uint32_t linePos = 0;
    char ch = line[linePos];

    while(ch != '\0')
    {
        if(ch == splitter)
            counter++;
        
        ch = line[linePos++];
    }

    return counter;
}

void tokenizer_with_splitter(const char* line, const char splitter, char** tokens)
{
    char buffer[100]; // 100: capacity for each token
    uint32_t linePos = 0;
    char ch = line[linePos];
    uint32_t tokenPos = 0;
    uint32_t tokenCount = 0;

    while(ch != '\0')
    {
        if ( ch == splitter || ch == '\0') {
            buffer[tokenPos] = '\0';
            tokens[tokenCount] = (char*)malloc(sizeof(char) * tokenPos);
            strcpy(tokens[tokenCount], buffer);
            tokenCount++;
            tokenPos = 0;
            ch = line[++linePos];
        } else {
            buffer[tokenPos++] = ch;
            ch = line[++linePos];
        }
    }

    // when ch is != 0 we deal with the last token here
    buffer[tokenPos] = '\0';
    tokens[tokenCount] = (char*)malloc(sizeof(char) * tokenPos);
    strcpy(tokens[tokenCount], buffer);
 
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