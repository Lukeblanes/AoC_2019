#include <stdio.h>
#include <stdint.h>

#include "../lib/vector.h"
#include "../lib/file.h"
#include "../lib/utils.h"

#define NUM_OPCODES 9
#define GENERATE_INTCODE_OUTPUT 0

// directions: north (1), south (2), west (3), and east (4)
uint32_t g_program_input = 0;

// Relative base used for relative mode
int64_t g_relative_base = 0;

// pc instruction to keep track of where we left off
int64_t g_PC = 0;

uint32_t g_pause_flag = 0;

uint32_t g_end_program = 0;

// used to track how many elements are in the output 
int32_t g_output_counter = 0;

// ONLY g_output_elems[0] is used
int32_t g_output_elems[3];

coordinate MOVES[4] = {{0, -1}, {0, 1}, {-1, 0}, {1, 0}};
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

// since it includes the \n we don't need to process anything
void print_grid(char* grid, uint32_t width, uint32_t height)
{
    for (uint32_t i = 0; i < width*height; i++)
        printf("%c", grid[i]);
}

uint32_t get_aligment_calculation(char* grid, uint32_t width, uint32_t height)
{
    uint32_t counter = 0;
    for (uint32_t i = 0; i < width*height; i++)
    {
        if(grid[i] == '#' && i > width && i < width*height - width)
        {
            if (grid[i - width] == '#' && grid[i + width] == '#' &&
                grid[i - 1] == '#' && grid[i + 1] == '#')
            {
                counter += (i % width) * (i / width);
            }
        }
    }
    return counter;
}

int main(int argc, char** argv)
{
    uint32_t size = 0;
    char **lines = file_read_with_tokenizer("input.txt", '\n', &size);

    uint32_t tokenCount = 0;
    char** tokens = tokenizer_with_delimiter(lines[0], ',', &tokenCount);

    // Store program
    IntVector *inputVector = iv_create_vector(10000);
    for(int i = 0; i < tokenCount; i++)
        iv_add_int(inputVector, atol(tokens[i]) );

    // create grid
    uint32_t grid_width = 1000;   
    uint32_t grid_size = grid_width * grid_width;   
    char *grid = malloc(sizeof(char*) * grid_size);

    // fill grid
    for (uint32_t i = 0; i < grid_size; i++)
        grid[i] = ' '; // black or unpainted
    
    // hard exit just in case
    uint32_t iterations = 0;

    g_program_input = 1; // initially move up
    uint32_t real_grid_width = 0; 
    uint32_t real_grid_height = 0;
    while (g_end_program == 0 && iterations <= grid_size)
    {
        run_intcode_program(inputVector->elements, g_PC, 0, 0, 0, 0); 

        if((uint8_t) g_output_elems[0] == '\n')
        {
            if (real_grid_width == 0)
                real_grid_width = iterations + 1; // we want to include the \n to the width

            real_grid_height++;
        }
        grid[iterations++] = (uint8_t) g_output_elems[0];
    }

//    print_grid(grid, real_grid_width, real_grid_height);
    uint32_t result = get_aligment_calculation(grid, real_grid_width, real_grid_height);
    printf("Sum of alignment params is %d\n", result); // output -> 4044

    // free resources
    free_char_pp(tokens, tokenCount);
    free_char_pp(lines, size);

    return 0;
}
