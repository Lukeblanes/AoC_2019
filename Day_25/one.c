#include <stdio.h>
#include <stdint.h>

#include "../lib/vector.h"
#include "../lib/file.h"
#include "../lib/utils.h"

#define GENERATE_INTCODE_OUTPUT 1
#include "opcodes.h"

// ITEMS TO PICK UP TO WEIGH THE SAME AS A DROID
// - food ration 
// - astrolabe 
// - weather machine 
// - ornament
// Output -> 4206594

int main(int argc, char** argv)
{
    uint32_t size = 0;
    char **lines = file_read_with_tokenizer("input.txt", '\n', &size);

    uint32_t tokencount = 0;
    char** tokens = tokenizer_with_delimiter(lines[0], ',', &tokencount);

    // store program
    IntVector *inputvector = iv_create_vector(30000);
    for(int i = 0; i < tokencount; i++)
        iv_add_int(inputvector, atol(tokens[i]));

    // input can't be higher than 50 chars
    char command[50];
    IntCodePC *droid = icpc_create(inputvector->elements, inputvector->capacity, 50, 1000, 0);

    while (1) 
    {
        // runs until it outputs command
        run_intcode_program(droid);
        if(droid->output_pause_flag == 1)
        {
            printf("Enter a command: ");

            char c;
            uint32_t len= 0;
            while( (c = getchar()) != '\n' )
                command[len++] = c;

            // add '\n' and null for string end
            command[len++] = '\n';
            command[len] = '\0';
 
            droid->input_elems_tracker = 0;

            for (uint32_t i = 0; i < strlen(command); i++)
                droid->input_elems[i] = (uint32_t) command[i];

            droid->output_pause_flag = 0;
        }
    }
    
    // free resources
    free_char_pp(tokens, tokencount);
    free_char_pp(lines, size);

    return 0;
}
