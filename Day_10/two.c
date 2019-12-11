#include <stdio.h>
#include <stdint.h>
#include <math.h> // for atan, compile linking math with -lm

#include "../lib/file.h" 
#include "../lib/utils.h" 
#include "../lib/vector.h" 

#define EMPTY_ELEM 12.48

// Can't reuse first approach so using angles from station to other asteroids
double get_angle(int32_t x_o, int32_t y_o, int32_t x_d, int32_t y_d)
{
    return atan2(x_d - x_o, y_d - y_o );
}

int main(int argc, char** argv)
{
    uint32_t size = 0;
    char **lines = file_read_with_tokenizer("input.txt", '\n', &size);

    CoordinateVector *asteroids = cv_create_coordinate_vector(100000);
    coordinate station_coord = {23, 29}; // Station from where we'll shoot the laser

    // Add asteroids to CoordinateVector
    uint32_t x_pos = 0;;
    coordinate aux;
    char c;
    for(int32_t i = 0; i < size; ++i)
    {
        c = lines[i][0];
        while(c != '\0')
        {
            if(c != '.' && !(station_coord.x == x_pos && station_coord.y == i) ) // not add station
            {
                aux.x = x_pos;
                aux.y = i;
                cv_add_coordinate(asteroids, aux);
            }
            c = lines[i][++x_pos];
        }
        x_pos = 0;
    }

    double angles[asteroids->size];

    // store angles for each asteroid
    for(uint32_t i = 0; i < asteroids->size; i++)
        angles[i] = get_angle(station_coord.x, station_coord.y, asteroids->coordinates[i].x, asteroids->coordinates[i].y);
    
    double ordered_set[asteroids->size]; // ordered and removing duplicates

    // populate with values to represent not filled positions
    for(uint32_t i = 0; i < asteroids->size; i++)
        ordered_set[i] = EMPTY_ELEM;  

    // sort in descending order and removing duplicates
    ordered_set[0] = angles[0]; // Initial insertion.
    uint32_t found_place = 0;
    uint32_t traverser ;
    for(uint32_t i = 1; i < asteroids->size; i++)
    {
        traverser = 1;
        found_place = 0;
        double to_insert = angles[i];
        while (found_place == 0)
        {
            if (ordered_set[traverser - 1] == EMPTY_ELEM) { // if highest, add as last element
                ordered_set[traverser - 1] = to_insert;
                found_place = 1;
            } else if(ordered_set[traverser-1] == to_insert) { // if found don't do anything
                found_place = 1;
            } else if(ordered_set[traverser-1] < to_insert) { // if unordered, order
                double previous = ordered_set[traverser-1];
                ordered_set[traverser-1] = to_insert;
                to_insert = previous;
            } 
            traverser++;
        }
    }

    // Find asteroid with angle ordered_set[199].
    // Search for asteroid that matches 200th angle.
    double result = 0.0; 
    for(uint32_t i = 0; i < asteroids->size; i++)
    {
        result = get_angle(station_coord.x, station_coord.y, asteroids->coordinates[i].x, asteroids->coordinates[i].y);
        if (result == ordered_set[199])
        {
            printf("Coordinate -> (%d, %d) with angle %f is the 200th destroyed asteroid. Calculation is %d.\n", asteroids->coordinates[i].x, asteroids->coordinates[i].y, result,
                    asteroids->coordinates[i].x * 100 + asteroids->coordinates[i].y); // Output -> 1110
            break;
        }
    }

    free_char_pp(lines, size);

    return 0;
}