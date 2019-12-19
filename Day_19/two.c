#include <stdio.h>
#include <stdint.h>

#include "../lib/vector.h"
#include "../lib/file.h"
#include "../lib/utils.h"

#define NUM_OPCODES 9
#define GENERATE_INTCODE_OUTPUT 0

int64_t g_relative_base = 0; // Relative base used for relative mode
int64_t g_PC = 0;// pc instruction to keep track of where we left off
uint32_t g_pause_flag = 0;
uint32_t g_end_program = 0;

// used to track how many elements are in the output 
int32_t g_output_counter = 0;

int32_t g_input_elems_size = 2;

// to keep track of current elem
int32_t g_input_tracker;
// ONLY g_output_elems[0] is used
int32_t g_input_elems[2];

// ONLY g_output_elems[0] is used
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
    g_relative_base = 0;
}

// Bruteforce solution
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

    // grid parameters, 2000 is enough to find solution
    uint32_t grid_width = 2000; 
    uint32_t grid_height = 2000; 

    // we have to copy original program since calling program modifies it
    int64_t* input = malloc(sizeof(int64_t) * inputVector->capacity);

    CoordinateVector *beam_positions = cv_create_coordinate_vector(1000); // There are 923

    // Todo: Better sol.: instead of finding all the beams and then finding the square,
    // we can do a square check here. In this case if there isn't a square in the first element of a line,
    // we can skip to the next line
    // Current time is 120 seconds with -02 optimization

    // Finding beam coordinates. Takes around 2 minutes
    for (uint32_t i = 0; i < grid_height; i++)
    {
        for (uint32_t j = 0; j < grid_width; j++)
        {
            g_end_program = 0;
            g_input_tracker = 0;
            while (g_end_program == 0)
            {
                g_input_elems[0] = j;
                g_input_elems[1] = i;
                memcpy(input, inputVector->elements, inputVector->capacity * sizeof(int64_t));
                    
                run_intcode_program(input, 0, 0, 0, 0, 0); 
                
                if (g_output_elems[0] == 1)
                    cv_add_coordinate(beam_positions, (coordinate) {.x = j, .y = i});
            }
        }
    }

    // Finding the the closest square of size SQUARE_SIZE closest to beam origin
    uint32_t found = 0;
    coordinate result = {-1, -1};
    #define SQUARE_SIZE 100
    printf("Beam coordinates found, now finding closest square of size %d\n", SQUARE_SIZE);
    for (uint32_t i = 0; i < beam_positions->size; i++)
    {
        found = 1;
        // we assume that current coordinate diagonally will always exist 
        if( cv_check_coordinate(beam_positions, (coordinate) {.x = beam_positions->coordinates[i].x + SQUARE_SIZE - 1, .y = beam_positions->coordinates[i].y}) == 0 || // 100 to the right
            cv_check_coordinate(beam_positions, (coordinate) {.x = beam_positions->coordinates[i].x, .y = beam_positions->coordinates[i].y + SQUARE_SIZE - 1 }) == 0)  // 100 down
                found = 0;

        // we found it so we store and exit
        if (found == 1)
        {
            result.x = beam_positions->coordinates[i].x;
            result.y = beam_positions->coordinates[i].y;
            break;
        }
    }
    
    printf("From coordinate(%d, %d) we obtain result %d\n", result.x, result.y, result.x * 10000 + result.y); // Output -> 6950903

    // free resources
    free_char_pp(tokens, tokenCount);
    free_char_pp(lines, size);

    return 0;
}
