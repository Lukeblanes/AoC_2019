#include <stdio.h>
#include <stdint.h>

#include "../lib/vector.h"
#include "../lib/file.h" 
#include "../lib/utils.h"

#define INPUT_LINES 2

int main(int argc, char** argv)
{
    char **lines = malloc(sizeof(char*) * INPUT_LINES);
    file_read_lines("input.txt", lines);

    uint32_t tokenCount1 = tokenizer_get_splitter_count(lines[0], ',') + 1;
    uint32_t tokenCount2 = tokenizer_get_splitter_count(lines[1], ',') + 1;
    char** tokens1 = malloc(sizeof(char*) * tokenCount1);
    char** tokens2 = malloc(sizeof(char*) * tokenCount2);

    tokenizer_with_splitter(lines[0], ',', tokens1);
    tokenizer_with_splitter(lines[1], ',', tokens2);

    CoordinateVector *wirepath1 = cv_create_coordinate_vector(10000);

    char movement;
    uint32_t moves;
    coordinate aux = {0, 0};
    int32_t* axis;
    int32_t dir;

    for(uint32_t i = 0; i < tokenCount1; ++i)
    {
        movement = tokens1[i][0];
        moves = atoi(tokens1[i] + 1); // by moving the pointer + 1 we skip the movement char

        if (movement == 'U') {
            axis = &aux.y;
            dir = 1;
        } else if (movement == 'D'){
            axis = &aux.y;
            dir = -1;
        } else if (movement == 'L'){
            axis = &aux.x;
            dir = -1;
        } else if (movement == 'R'){
            axis = &aux.x;
            dir = 1;
        }

        for (uint32_t j = 0; j < moves; j++)
        {
            *axis += dir;
            cv_add_coordinate(wirepath1, aux);
        }
    }

    // reset aux
    aux.x = 0;
    aux.y = 0;

    int steps2 = 0;
    int steps1 = 0;
    int least_steps = 1000000;

    for(uint32_t i = 0; i < tokenCount2; ++i)
    {
        movement = tokens2[i][0];
        moves = atoi(tokens2[i] + 1);

        if (movement == 'U') {
            axis = &aux.y;
            dir = 1;
        } else if (movement == 'D'){
            axis = &aux.y;
            dir = -1;
        } else if (movement == 'L'){
            axis = &aux.x;
            dir = -1;
        } else if (movement == 'R'){
            axis = &aux.x;
            dir = 1;
        }

        for (uint32_t j = 0; j < moves; j++)
        {
            *axis += dir;
            steps2++;

            // steps2 <= least_steps is so that we don't perform get_pos when we know we can't find a better least_pos
            if ( steps2 <= least_steps && (steps1 = cv_get_pos(wirepath1, aux)) != 0 )
            {
                steps1++; // Adjust to +1 to get steps instead of position
                if ( steps1 + steps2 < least_steps)
                    least_steps = steps1 + steps2;
            }
        }
    }

    printf("Interception with the least steps took %d steps\n", least_steps);

    // Freeing resources
    free_char_pp(tokens1, tokenCount1);
    free_char_pp(tokens2, tokenCount2);
    free_char_pp(lines, INPUT_LINES);
    
    return 0;
}