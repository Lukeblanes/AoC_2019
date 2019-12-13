#include <stdio.h>
#include <stdint.h>

#include "../lib/vector.h"
#include "../lib/file.h"
#include "../lib/utils.h"

#define NUM_OPCODES 9
#define GENERATE_INTCODE_OUTPUT 0

// Used to specify the only input of the program
uint32_t g_program_input = 0;

// Relative base used for relative mode
int64_t g_relative_base = 0;

// pc instruction to keep track of where we left off
int64_t g_PC = 0;

uint32_t g_pause_flag = 0;

uint32_t g_end_program = 0;

// used to track how many elements are in the output 
int32_t g_output_counter = 0;
int32_t g_output_elems[3];

#include "opcodes.h"

void (*OP_functions[NUM_OPCODES])(int64_t*, int64_t*, int64_t, int64_t, int64_t, uint64_t) =
                                     { OP_01, OP_02, OP_03, OP_04, OP_05, OP_06, OP_07, OP_08, OP_09};
uint64_t OP_jumpValue[NUM_OPCODES] = { 4,     4,     2,     2,     3,     3,     4,     4,     2};

int64_t get_mode_index(int64_t* input, int64_t PC, int64_t* a, int64_t* b, int64_t *c)
{
    int64_t OP = input[PC];
    if(OP >= 20000)      *c = input[PC + 3] + g_relative_base; // Relative mode
    else if(OP >= 10000) *c = PC+3;                          // Immediate mode
    else                 *c = input[PC+3];                   // Position mode

    OP %= 10000;
    if(OP >= 2000)       *b = input[PC + 2] + g_relative_base;
    else if(OP >= 1000)  *b = PC+2;
    else                 *b = input[PC+2];

    OP %= 1000;
    if(OP >= 200)        *a = input[PC + 1] + g_relative_base;
    else if(OP >= 100)   *a = PC+1;
    else                 *a = input[PC+1];

    return OP % 100;
}

void run_intcode_program(int64_t* input, int64_t PC, int64_t OP, int64_t a, int64_t b, int64_t c)
{
    while(input[PC] != 99 && g_pause_flag == 0)
    {
        OP = get_mode_index(input, PC, &a, &b, &c);

        if (OP < 1 || OP > NUM_OPCODES)
        {
            printf("Something went wrong. PC = %ld. input[PC] = %ld. OP = %ld. a,b,c = (%ld, %ld), (%ld, %ld), (%ld, %ld)\n",
                    PC, input[PC], OP, input[PC+1], input[a], input[PC+2], input[b], input[PC+3], input[c]);
            exit(0);
        }
        else
        {
            OP_functions[OP-1](input, &PC, a, b, c, OP_jumpValue[OP-1]);
        }
    }

    if(input[PC] == 99)
        g_end_program = 1;

    // Reset flags
    g_pause_flag = 0;
    g_output_counter = 0;
}

int main(int argc, char** argv)
{
    uint32_t size = 0;
    char **lines = file_read_with_tokenizer("input.txt", '\n', &size);

    uint32_t tokenCount = 0;
    char** tokens = tokenizer_with_delimiter(lines[0], ',', &tokenCount);

    IntVector *inputVector = iv_create_vector(10000);

    for(int i = 0; i < tokenCount; i++)
        iv_add_int(inputVector, atol(tokens[i]) );

    CoordinateVector *coords = cv_create_coordinate_vector(10000);
    char tiles[10000];
    coordinate aux;
    while (g_end_program == 0)
    {
        run_intcode_program(inputVector->elements, g_PC, 0, 0, 0, 0); 

        aux.x = g_output_elems[0];
        aux.y = g_output_elems[1];
        cv_add_coordinate(coords, aux);

        uint32_t position = coords->size - 1;
        if(g_output_elems[2] == 0)
            tiles[position] = '.'; // Empty
        else if(g_output_elems[2] == 1)
            tiles[position] = '#'; // Wall
        else if(g_output_elems[2] == 2)
            tiles[position] = 'B'; // Block 
        else if(g_output_elems[2] == 3)
            tiles[position] = '_'; // horizontal paddle
        else if(g_output_elems[2] == 4)
            tiles[position] = 'o'; // ball
        else 
            printf("Something went wrong!\n");
        
    }

    uint32_t block_count= 0;
    for (uint32_t i = 0; i < coords->size; i++)
        if(tiles[i] == 'B')
            block_count++;
    
    printf("There are %d blocks.\n", block_count); // Output -> 242

    // free resources
    free_char_pp(tokens, tokenCount);
    free_char_pp(lines, size);

    return 0;
}
