#include <stdio.h>
#include <stdint.h>

#include "../lib/vector.h" 
#include "../lib/file.h" 
#include "../lib/utils.h" 

#define NUM_OPCODES 8
#define GENERATE_INTCODE_OUTPUT 0

// Used to pass the phase setting to the amp
uint32_t g_phase_num = 0;

// Used as the input signal. Initially 0 but then it will be the output of the previous amp.
uint32_t g_input_signal = 0;

// When it's the first iteration, it indicates if we read from phase or input
uint32_t g_input_flag = 0;

// Indicates when to pause current amp and pass the input to the next amp (for part 2)
uint32_t g_end_wait = 0;

// Indicates the first iteration of the feedback loop so that after the first iteration we don't input the phase (for part 2)
uint32_t g_first_iteration= 0;

#include "opcodes.h"

void (*OP_functions[NUM_OPCODES])(int32_t*, int32_t*, int32_t, int32_t, int32_t, uint32_t) = 
                                     { OP_01, OP_02, OP_03, OP_04, OP_05, OP_06, OP_07, OP_08};
uint32_t OP_jumpValue[NUM_OPCODES] = { 4,     4,     2,     2,     3,     3,     4,     4};

uint32_t permutations[120][5] = {{0, 1, 2, 3, 4}, {0, 1, 2, 4, 3}, {0, 1, 3, 2, 4}, {0, 1, 3, 4, 2}, {0, 1, 4, 2, 3}, {0, 1, 4, 3, 2}, {0, 2, 1, 3, 4}, {0, 2, 1, 4, 3}, {0, 2, 3, 1, 4}, {0, 2, 3, 4, 1}, {0, 2, 4, 1, 3}, {0, 2, 4, 3, 1}, {0, 3, 1, 2, 4}, {0, 3, 1, 4, 2}, {0, 3, 2, 1, 4}, {0, 3, 2, 4, 1}, {0, 3, 4, 1, 2}, {0, 3, 4, 2, 1}, {0, 4, 1, 2, 3}, {0, 4, 1, 3, 2}, {0, 4, 2, 1, 3}, {0, 4, 2, 3, 1}, {0, 4, 3, 1, 2}, {0, 4, 3, 2, 1}, {1, 0, 2, 3, 4}, {1, 0, 2, 4, 3}, {1, 0, 3, 2, 4}, {1, 0, 3, 4, 2}, {1, 0, 4, 2, 3}, {1, 0, 4, 3, 2}, {1, 2, 0, 3, 4}, {1, 2, 0, 4, 3}, {1, 2, 3, 0, 4}, {1, 2, 3, 4, 0}, {1, 2, 4, 0, 3}, {1, 2, 4, 3, 0}, {1, 3, 0, 2, 4}, {1, 3, 0, 4, 2}, {1, 3, 2, 0, 4}, {1, 3, 2, 4, 0}, {1, 3, 4, 0, 2}, {1, 3, 4, 2, 0}, {1, 4, 0, 2, 3}, {1, 4, 0, 3, 2}, {1, 4, 2, 0, 3}, {1, 4, 2, 3, 0}, {1, 4, 3, 0, 2}, {1, 4, 3, 2, 0}, {2, 0, 1, 3, 4}, {2, 0, 1, 4, 3}, {2, 0, 3, 1, 4}, {2, 0, 3, 4, 1}, {2, 0, 4, 1, 3}, {2, 0, 4, 3, 1}, {2, 1, 0, 3, 4}, {2, 1, 0, 4, 3}, {2, 1, 3, 0, 4}, {2, 1, 3, 4, 0}, {2, 1, 4, 0, 3}, {2, 1, 4, 3, 0}, {2, 3, 0, 1, 4}, {2, 3, 0, 4, 1}, {2, 3, 1, 0, 4}, {2, 3, 1, 4, 0}, {2, 3, 4, 0, 1}, {2, 3, 4, 1, 0}, {2, 4, 0, 1, 3}, {2, 4, 0, 3, 1}, {2, 4, 1, 0, 3}, {2, 4, 1, 3, 0}, {2, 4, 3, 0, 1}, {2, 4, 3, 1, 0}, {3, 0, 1, 2, 4}, {3, 0, 1, 4, 2}, {3, 0, 2, 1, 4}, {3, 0, 2, 4, 1}, {3, 0, 4, 1, 2}, {3, 0, 4, 2, 1}, {3, 1, 0, 2, 4}, {3, 1, 0, 4, 2}, {3, 1, 2, 0, 4}, {3, 1, 2, 4, 0}, {3, 1, 4, 0, 2}, {3, 1, 4, 2, 0}, {3, 2, 0, 1, 4}, {3, 2, 0, 4, 1}, {3, 2, 1, 0, 4}, {3, 2, 1, 4, 0}, {3, 2, 4, 0, 1}, {3, 2, 4, 1, 0}, {3, 4, 0, 1, 2}, {3, 4, 0, 2, 1}, {3, 4, 1, 0, 2}, {3, 4, 1, 2, 0}, {3, 4, 2, 0, 1}, {3, 4, 2, 1, 0}, {4, 0, 1, 2, 3}, {4, 0, 1, 3, 2}, {4, 0, 2, 1, 3}, {4, 0, 2, 3, 1}, {4, 0, 3, 1, 2}, {4, 0, 3, 2, 1}, {4, 1, 0, 2, 3}, {4, 1, 0, 3, 2}, {4, 1, 2, 0, 3}, {4, 1, 2, 3, 0}, {4, 1, 3, 0, 2}, {4, 1, 3, 2, 0}, {4, 2, 0, 1, 3}, {4, 2, 0, 3, 1}, {4, 2, 1, 0, 3}, {4, 2, 1, 3, 0}, {4, 2, 3, 0, 1}, {4, 2, 3, 1, 0}, {4, 3, 0, 1, 2}, {4, 3, 0, 2, 1}, {4, 3, 1, 0, 2}, {4, 3, 1, 2, 0}, {4, 3, 2, 0, 1}, {4, 3, 2, 1, 0}};

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

void run_intcode_program(int32_t* input, int32_t PC, int32_t OP, uint32_t phase, int32_t a, int32_t b, int32_t c)
{
    g_phase_num = phase;
    while(input[PC] != 99 && g_end_wait == 0) 
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
    g_end_wait = 0; 
}

int main(int argc, char** argv)
{
    uint32_t size = 0;
    char **lines = file_read_with_tokenizer("input.txt", '\n', &size);

    uint32_t tokenCount = 0;
    char** tokens = tokenizer_with_delimiter(lines[0], ',', &tokenCount);

    IntVector *inputVector = iv_create_vector(10000);

    for(int i = 0; i < tokenCount; i++)
        iv_add_int(inputVector, atoi(tokens[i]));

    int32_t* input = malloc(sizeof(int32_t) * inputVector->size);

    uint32_t best = 0;
    for(uint32_t j = 0; j < 120; j++)
    {
        for(uint32_t i = 0; i < 5; i++)
        {
            memcpy(input, inputVector->elements, inputVector->size * sizeof(int32_t));
            run_intcode_program(input, 0, 0, permutations[j][i], 0, 0, 0);
        }
        if(best < g_input_signal)
            best = g_input_signal;
        g_input_signal = g_input_flag = 0;
    }

    printf("Best is %d\n", best); // Output -> 92663 

    // free resources
    free_char_pp(tokens, tokenCount);
    free_char_pp(lines, size);

    return 0;
}