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

// Input 
int32_t g_input_tracker = 0;
int32_t g_input_elems_size;
char g_input_elems[16 * 8]; // [ (maximum number of instruction + 1) *  maximum innstruction width ]

// Output
#define OUTPUT_ELEM 10000
int32_t g_output_tracker = 0;
int32_t g_output_max = 0; // same as tracker but isn't destroyed when program ends
uint32_t g_output_elems[OUTPUT_ELEM];

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
    g_output_tracker = 0;
    g_relative_base = 0;
}

void print_output()
{
    for (uint32_t i = 0; i < g_output_max; i++)
    {
        if(g_output_elems[i] > 255)
            printf("%d\n", g_output_elems[i]);
        else
            printf("%c", g_output_elems[i]);
    }
}

int main(int argc, char** argv)
{
    cmd_input_validate(argc, argv, "SpringScript_program", 2);

    uint32_t size = 0;
    char **lines = file_read_with_tokenizer("input.txt", '\n', &size);

    uint32_t tokenCount = 0;
    char** tokens = tokenizer_with_delimiter(lines[0], ',', &tokenCount);

    // Store program
    IntVector *inputVector = iv_create_vector(10000);
    for(int i = 0; i < tokenCount; i++)
        iv_add_int(inputVector, atol(tokens[i]) );

    // Load SpringScript Program
    FILE *file = fopen(argv[1], "r");
    char ch;
    g_input_elems_size = 0;
    if (file == NULL) {
        printf("Couldn't open the file.");
    } else {
        while( (ch = fgetc(file)) != EOF)
            g_input_elems[g_input_elems_size++] = ch;
    }
    fclose(file);

    // Strategy for both programs is to jump only 
    // if we know we will be able to jump to another location after the initial jump
    // if there are multiple distances without a hole, we don't have to jump (normal robot execution will move it)

    // Print Program
    printf("PROGRAM:\n");
    for (uint32_t i = 0; i < g_input_elems_size; i++)
        printf("%c", g_input_elems[i]);
    printf("PROGRAM END\n");

    while (g_end_program == 0)
        run_intcode_program(inputVector->elements, 0, 0, 0, 0, 0); 
    // Part 1 output -> 19354818
    // Part 2 output -> 1143787220

    print_output();

    // free resources
    free_char_pp(tokens, tokenCount);
    free_char_pp(lines, size);

    return 0;
}
