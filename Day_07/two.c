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

// Indicates when to pause current amp and pass the input to the next amp
uint32_t g_end_wait = 0;

// Indicates the first iteration of the feedback loop so that after the first iteration we don't input the phase
uint32_t g_first_iteration= 0;

// Indicates when opcode 99 is found to run feedback loop with different permutation
uint32_t g_program_end = 0;

#include "opcodes.h"

void (*OP_functions[NUM_OPCODES])(int32_t*, int32_t*, int32_t, int32_t, int32_t, uint32_t) = 
                                     { OP_01, OP_02, OP_03, OP_04, OP_05, OP_06, OP_07, OP_08};
uint32_t OP_jumpValue[NUM_OPCODES] = { 4,     4,     2,     2,     3,     3,     4,     4};

uint32_t permutations[120][5] = {{5, 6, 7, 8, 9}, {5, 6, 7, 9, 8}, {5, 6, 8, 7, 9}, {5, 6, 8, 9, 7}, {5, 6, 9, 7, 8}, {5, 6, 9, 8, 7}, {5, 7,6, 8, 9}, {5, 7, 6, 9, 8}, {5, 7, 8, 6, 9}, {5, 7, 8, 9, 6}, {5, 7, 9, 6, 8}, {5, 7, 9, 8, 6}, {5, 8, 6, 7, 9}, {5, 8,6, 9, 7},{5, 8, 7, 6, 9}, {5, 8, 7, 9, 6}, {5, 8, 9, 6, 7}, {5, 8, 9, 7, 6}, {5, 9, 6, 7, 8}, {5, 9, 6, 8, 7}, {5, 9,7, 6, 8}, {5, 9, 7, 8, 6}, {5, 9, 8, 6, 7}, {5, 9, 8, 7, 6}, {6, 5, 7, 8, 9}, {6, 5, 7, 9, 8}, {6, 5, 8, 7, 9}, {6, 5,8, 9, 7}, {6, 5, 9, 7, 8}, {6, 5, 9, 8, 7}, {6, 7, 5, 8, 9}, {6, 7, 5, 9, 8}, {6, 7, 8, 5, 9}, {6, 7, 8, 9, 5}, {6, 7,9, 5, 8}, {6, 7, 9, 8, 5}, {6, 8, 5, 7, 9}, {6, 8, 5, 9, 7}, {6, 8, 7, 5, 9}, {6, 8, 7, 9, 5}, {6, 8, 9, 5, 7}, {6, 8,9, 7, 5}, {6,9, 5, 7, 8}, {6, 9, 5, 8, 7}, {6, 9, 7, 5, 8}, {6, 9, 7, 8, 5}, {6, 9, 8, 5, 7}, {6, 9, 8, 7, 5}, {7, 5,6, 8, 9}, {7, 5, 6, 9, 8}, {7, 5, 8, 6, 9}, {7, 5, 8, 9, 6}, {7, 5, 9, 6, 8}, {7, 5, 9, 8, 6}, {7, 6, 5, 8, 9}, {7, 6,5, 9, 8}, {7, 6, 8, 5, 9}, {7, 6, 8, 9, 5}, {7, 6, 9, 5, 8}, {7, 6, 9, 8, 5}, {7, 8, 5, 6, 9}, {7, 8, 5, 9, 6}, {7, 8,6, 5, 9}, {7, 8,6, 9, 5}, {7, 8, 9, 5, 6}, {7, 8, 9, 6, 5}, {7, 9, 5, 6, 8}, {7, 9, 5, 8, 6}, {7, 9, 6, 5, 8}, {7, 9,6, 8, 5}, {7, 9, 8, 5, 6}, {7, 9, 8, 6, 5}, {8, 5, 6, 7, 9}, {8, 5, 6, 9, 7}, {8, 5, 7, 6, 9}, {8, 5, 7, 9, 6}, {8, 5,9, 6, 7}, {8, 5, 9, 7, 6}, {8, 6, 5, 7, 9}, {8, 6, 5, 9, 7}, {8, 6, 7, 5, 9}, {8, 6, 7, 9, 5}, {8, 6, 9, 5, 7}, {8, 6,9, 7, 5}, {8, 7, 5,6, 9}, {8, 7, 5, 9, 6}, {8, 7, 6, 5, 9}, {8, 7, 6, 9, 5}, {8, 7, 9, 5, 6}, {8, 7, 9, 6, 5}, {8, 9,5, 6, 7}, {8, 9, 5, 7, 6}, {8, 9, 6, 5, 7}, {8, 9, 6, 7, 5}, {8, 9, 7, 5, 6}, {8, 9, 7, 6, 5}, {9, 5, 6, 7, 8}, {9, 5,6, 8, 7}, {9, 5, 7, 6, 8}, {9, 5, 7, 8, 6}, {9, 5, 8, 6, 7}, {9, 5, 8, 7, 6}, {9, 6, 5, 7, 8}, {9, 6, 5, 8, 7}, {9, 6,7, 5, 8}, {9, 6, 7, 8,5}, {9, 6, 8, 5, 7}, {9, 6, 8, 7, 5}, {9, 7, 5, 6, 8}, {9, 7, 5, 8, 6}, {9, 7, 6, 5, 8}, {9, 7,6, 8, 5}, {9, 7, 8, 5, 6}, {9, 7, 8, 6, 5}, {9, 8, 5, 6, 7}, {9, 8, 5, 7, 6}, {9, 8, 6, 5, 7}, {9, 8, 6, 7, 5}, {9, 8,7, 5, 6}, {9, 8, 7, 6, 5}};

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


void run_intcode_program(int32_t* input, int32_t *PC_pointer, int32_t OP, uint32_t phase, int32_t a, int32_t b, int32_t c, int32_t amp_id)
{
    int32_t PC = *PC_pointer;
        
    g_phase_num = phase;
    g_input_flag = 0;
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

    if(input[PC] == 99)
    {
        g_program_end = 1;
    }

    // reset exit condition and set pc pointer
    g_end_wait = 0;
    *PC_pointer = PC;
}

int main(int argc, char** argv)
{
    uint32_t size = 0;
    char **lines = file_read_with_tokenizer("input.txt", '\n', &size);

    uint32_t tokenCount = 0;
    char** tokens = tokenizer_with_delimiter(lines[0], ',', &tokenCount);

    IntVector *inputVector = iv_create_coordinate_vector(10000);

    for(int i = 0; i < tokenCount; i++)
        iv_add_int(inputVector, atoi(tokens[i]));

    // used to store the state of the amp before passing to the next one
    int32_t * amps[5];
    for(int i = 0; i < 5; i++)
        amps[i] = malloc(sizeof(int32_t) * inputVector->size);

    int32_t amps_PC_states[5]; 

    int32_t combination[5];

    uint32_t best = 0;
    for(uint32_t j = 0; j < 120; j++)
    {
        g_program_end = g_first_iteration = g_input_signal = 0; // Reset for next permutation
        for(uint32_t i = 0; i < 5; i++)
        {
            memcpy(amps[i], inputVector->elements, inputVector->size * sizeof(int32_t));
            amps_PC_states[i] = 0;
        }

        while(g_program_end == 0)
        {
            for(uint32_t i = 0; i < 5; i++)
            {
                int32_t *input = amps[i];
                run_intcode_program(input, &amps_PC_states[i], 0, permutations[j][i], 0, 0, 0, i);
            }
            g_first_iteration = 1; // After the first iteration we don't imput the signal anymore just the output from the previous amp
        }

        if(best < g_input_signal)
        {
            best = g_input_signal;
            for(int i = 0; i < 5; i++)
                combination[i] = permutations[j][i];
        }
    }
    
    // Correct output -> 14365052
    printf("Best is %d with combination %d, %d, %d, %d, %d\n", best, combination[0],combination[1],combination[2],combination[3],combination[4]);

    // free resources
    free_char_pp(tokens, tokenCount);
    free_char_pp(lines, size);

    return 0;
}