#include <stdio.h>
#include <stdint.h>

#include "../lib/vector.h" 
#include "../lib/file.h" 
#include "../lib/utils.h" 

#define INPUT_LINES 1
#define PROGRAM_INPUT 5
#define NUM_OPCODES 8

#include "opcodes.h"

void (*OP_functions[NUM_OPCODES])(int32_t*, int32_t*, int32_t, int32_t, int32_t, uint32_t) = 
                                     { OP_01, OP_02, OP_03, OP_04, OP_05, OP_06, OP_07, OP_08};
uint32_t OP_jumpValue[NUM_OPCODES] = { 4,     4,     2,     2,     3,     3,     4,     4};

int32_t get_mode_index(int32_t* input, int32_t PC, int32_t* a, int32_t* b, int32_t *c)
{
    int32_t OP = input[PC];
    if(OP > 10000) *c = PC+3;
    else           *c = input[PC+3];

    OP %= 10000;
    if(OP > 1000) *b = PC+2;
    else          *b = input[PC+2];

    OP %= 1000;
    if(OP > 100) *a = PC+1;
    else         *a = input[PC+1];

    return OP % 100; 
}

int main(int argc, char** argv)
{
    char **lines = malloc(sizeof(char*) * INPUT_LINES);
    file_read_lines("input.txt", lines);

    uint32_t tokenCount = tokenizer_get_splitter_count(lines[0], ',') + 1;
    char** tokens = malloc(sizeof(char*) * tokenCount);

    tokenizer_with_splitter(lines[0], ',', tokens);

    IntVector *inputVector = iv_create_coordinate_vector(10000);

    for(int i = 0; i < tokenCount; i++)
    {
        iv_add_int(inputVector, atoi(tokens[i]));
    }

    int32_t a, b, c;
    int32_t* input = inputVector->elements;
    int32_t PC = 0;
    int32_t OP = 0;
    while(input[PC] != 99)
    {
        OP = get_mode_index(input, PC, &a, &b, &c);

        if (OP < 1 || OP > NUM_OPCODES)
        {
            printf("Something went wrong. PC = %d. input[PC] = %d. OP = %d. a,b,c = (%d, %d), (%d, %d), (%d, %d)\n",
                    PC, input[PC], OP, input[PC+1], input[a], input[PC+2], input[b], input[PC+3], input[c]);
            exit(0);
        }
        else
        {
            OP_functions[OP-1](input, &PC, a, b, c, OP_jumpValue[OP-1]);
        }
    }

    // free resources
    free_char_pp(tokens, tokenCount);
    free_char_pp(lines, INPUT_LINES);

    return 0;
}