#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define COLUMN_LIMIT 2048

char** file_read_with_tokenizer(const char* filename, const char delimiter, uint32_t *token_count)
{
    FILE *file = fopen(filename, "r");
    char ch;
    uint32_t size = COLUMN_LIMIT;
    // dynamically allocated in the case that it surpasses max COLUMN_LIMIT
    char *buffer = malloc(sizeof(char) * size); 
    char *deleter = buffer; // to delete memory since buffer will be modified

    // First pass where we store all chars and count tokens
    if (file == NULL) {
        printf("Couldn't open the file.");
    } else {
        uint32_t pos = 0;
        while( (ch = fgetc(file)) != EOF)
        {
            if(pos == size)
            {
                printf("Reallocating memory. Not enough size.");
                size *= 2;
                buffer = deleter = realloc(buffer, size);
                if(buffer == NULL)
                {
                    printf("Error reallocating buffer\n");
                    exit(-1);
                }
            }
            if ( ch == delimiter)
                (*token_count)++;

            buffer[pos++] = ch;
        }
        if(buffer[pos-1] != delimiter)
            (*token_count)++; // One extra count since tokens = delimiters + 1
    }
    fclose(file);

    // Second pass where we create the array of strings since we know the tokenCount
    char** lines = malloc(sizeof(char*) * (*token_count));
    const char delimiter_string[2] = {delimiter, '\0'};

    buffer = strtok(buffer, delimiter_string); // first call of strtok need buffer to be passed

    uint32_t counter = 0;
    while( buffer!= NULL)
    {
        if(counter == (*token_count))
        {
            printf("Inserting more tokens than counted.\n");
            exit(-1);
        }

        lines[counter] = malloc(sizeof(char) * strlen(buffer) + 1);
        strcpy(lines[counter], buffer);
        
        buffer = strtok(NULL, delimiter_string);
        counter++;
    }
    free(deleter);
    return lines;
}
