#include <stdio.h>
#include <stdint.h>

#include "../lib/file.h" 
#include "../lib/utils.h" 
#include "../lib/vector.h" 

coordinate get_smallest_valid_vector(int32_t o_x, int32_t o_y, int32_t d_x, int32_t d_y)
{
    // get direction vector
    int32_t x = d_x - o_x;
    int32_t y = d_y - o_y;

    coordinate to_return = {0, 0};
    // we obtain the absolute highest to mantain sign during division
    uint32_t abs_x = x < 0 ? x*(-1) : x;
    uint32_t abs_y = y < 0 ? y*(-1) : y;
    uint32_t max_to_check = abs_x > abs_y ? abs_x : abs_y; // get max value
    
    for(int32_t i = max_to_check; i > 0; --i)
    {
        if( (x % i == 0 ) && (y % i == 0)) 
        {
            to_return.x = x / i; 
            to_return.y = y / i;
            return to_return;
        }
    }
    return to_return;
}

int main(int argc, char** argv)
{
    uint32_t size = 0;
    char **lines = file_read_with_tokenizer("input.txt", '\n', &size);
    
    CoordinateVector *asteroids = cv_create_coordinate_vector(100000);

    // Add asteroids to CoordinateVector
    uint32_t x_pos = 0;;
    coordinate aux;
    char c;
    for(int32_t i = 0; i < size; ++i)
    {
        c = lines[i][0];
        while(c != '\0')
        {
            if(c != '.')
            {
                aux.x = x_pos;
                aux.y = i;
                cv_add_coordinate(asteroids, aux);
            }
            c = lines[i][++x_pos];
        }
        x_pos = 0;
    }

    // Find all "unit" vectors for each asteroid. if different asteroid's unit vectors coincide we don't count
    CoordinateVector *normalized_vectors = cv_create_coordinate_vector(100000);
    coordinate unit_vector_aux;

    uint32_t best = 0;
    coordinate best_coordinate;
    for(uint32_t i = 0; i < asteroids->size; ++i)
    {
        coordinate current = asteroids->coordinates[i];
        for(uint32_t j = 0; j < asteroids->size; ++j)
        {
            if( !(current.x == asteroids->coordinates[j].x && current.y == asteroids->coordinates[j].y) ) // to skip current
            {
                unit_vector_aux = get_smallest_valid_vector(current.x, current.y, asteroids->coordinates[j].x, asteroids->coordinates[j].y);
                uint32_t found = cv_check_coordinate(normalized_vectors, unit_vector_aux);
                if(found == 0)
                    cv_add_coordinate(normalized_vectors, unit_vector_aux);
            }
        }
        if(normalized_vectors->size > best)
        {
            best = normalized_vectors->size;
            best_coordinate = current;
        }
        cv_clear(normalized_vectors);
    }
    printf("From the best station at (%d, %d) we can see %d asteroids.\n", best_coordinate.x, best_coordinate.y, best); // Output -> 263

    // free resources
    free_char_pp(lines, size);

    return 0;
}
