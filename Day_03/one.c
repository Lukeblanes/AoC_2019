#include <stdio.h>
#include <stdint.h>

#include "../lib/vector.h" 
#include "../lib/file.h" 
#include "../lib/utils.h" 

#define INPUT_LINES 2

int main(int argc, char** argv)
{
    char **lines = malloc(sizeof(char*) * INPUT_LINES);
    // Correct output for input.txt is 308 
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

    int a, b;

    coordinate shortestCoord = {1000000, 1000000};
    uint32_t shortestDistance = 1000000 + 1000000;

    // check intersections
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
            if (aux.x < 0) a = aux.x * - 1; else a = aux.x;
            if (aux.y < 0) b = aux.y * - 1; else b = aux.y;

            // a+b > shortestDistance is so that we don't perform the check if we know that the current coordinate can't be the closest
            if ( a+b < shortestDistance && cv_check_coordinate(wirepath1, aux) )
            {
                if (shortestDistance > a + b)
                {
                    shortestCoord.x = aux.x;
                    shortestCoord.y = aux.y;
                    shortestDistance = a + b;
                }
            }
        }
    }

    printf("Shortest Coord is (%d, %d) with distance %d\n", shortestCoord.x, shortestCoord.y, shortestDistance);

    // free resources
    free_char_pp(tokens1, tokenCount1);
    free_char_pp(tokens2, tokenCount2);
    free_char_pp(lines, INPUT_LINES);

    return 0;
}