#include <stdio.h>
#include <stdint.h>

#include "../lib/file.h" 
#include "../lib/utils.h" 
#include "../lib/vector.h" 

uint32_t GRID_WIDTH, GRID_HEIGHT;

// Where ? is located
#define MIDDLE_POS 12

// number of iteration
#define STEPS 200

// We assume that at each step, we might decrease or increase the depth at which bugs are.
#define DEPTH STEPS*2

// We start at the middle of the LEVELS array we create
#define START STEPS

char LEVELS[DEPTH][25];

// array positions around the center(?) to use 
#define INTERIOR_UP 7
#define INTERIOR_LEFT 11 
#define INTERIOR_RIGHT 13
#define INTERIOR_DOWN 17

// arrays to check the special cases and the special cases corresponding positions
#define INTERIOR_CASES 4
uint32_t SPECIAL_CASES_INDEX[INTERIOR_CASES] = {INTERIOR_UP, INTERIOR_DOWN, INTERIOR_LEFT, INTERIOR_RIGHT};
uint32_t SPECIAL_CASES[INTERIOR_CASES][5] = {{0, 1, 2, 3, 4}, {20, 21, 22, 23, 24}, {0, 5, 10, 15, 20}, {4, 9, 14, 19, 24}};

// used to know the range of depths we have to study
uint32_t MIN_DEPTH, MAX_DEPTH;

void initialize_levels(char* starting_level)
{
    for (uint32_t i = 0; i < DEPTH; i++)
    {
        for (uint32_t j = 0; j < 25; j++)
        {
            if(j == 12)
                LEVELS[i][j] = '?';
            else if (i == START)
                LEVELS[i][j] = starting_level[j];
            else
                LEVELS[i][j] = '.';
        }
    }
}

uint32_t count_bugs()
{
    uint32_t count = 0;
    for (uint32_t i = 0; i < DEPTH; i++)
        for (uint32_t j = 0; j < 25; j++)
            if(LEVELS[i][j] == '#')
                count++;
    return count;
}

// Improvable spaghetti code
void generate_next_state()
{
    // store initial depths incase they are modified
    uint32_t initial_max = MAX_DEPTH;
    uint32_t initial_min = MIN_DEPTH; 

    uint32_t levels = (MAX_DEPTH+1) - (MIN_DEPTH-1) + 1; // number of levels we are generating

    uint32_t adjacent_bugs; // adjacent bugs for a specific position
    uint32_t adjacent_bugs_level; // if adjacent bugs have been generated in a level, this is set to modify MAX or MIN Depth
    char new_map[levels][25]; // where we copy the next generated state

    for (uint32_t j = initial_min-1; j <= initial_max+1; j++)
    {
        adjacent_bugs_level = 0;
        for (uint32_t i = 0; i < 25; i++)
        {
            adjacent_bugs = 0;
            // CHECK IN EVERY DIRECTION
            if(i >= GRID_WIDTH) { // we can check up in current level
                if(LEVELS[j][i-GRID_WIDTH] == '#')
                    adjacent_bugs++;
            } else { // we check up in a higher level
                if(LEVELS[j-1][INTERIOR_UP] == '#')
                    adjacent_bugs++;
            }

            if(i < GRID_WIDTH * (GRID_HEIGHT-1)) // we can check down
            {
                if(LEVELS[j][i+GRID_WIDTH] == '#')
                    adjacent_bugs++;
            } else { // we check down in a higher level
                if(LEVELS[j-1][INTERIOR_DOWN] == '#')
                    adjacent_bugs++;
            }

            if(i % GRID_WIDTH != 0) { // we can check left
                if(LEVELS[j][i-1] == '#')
                    adjacent_bugs++;
            } else { // we check left in a higher level
                if(LEVELS[j-1][INTERIOR_LEFT] == '#')
                    adjacent_bugs++;
            }

            if((i+1) % GRID_WIDTH != 0) { // we can check right 
                if(LEVELS[j][i+1] == '#')
                    adjacent_bugs++;
            } else { // we check right in a higher level
                if(LEVELS[j-1][INTERIOR_RIGHT] == '#')
                    adjacent_bugs++;
            }

            // CHECK SPECIAL CASES THAT LOOK DOWN A LEVEL
            for (uint32_t k = 0; k < INTERIOR_CASES; k++)
            {
                if(i == SPECIAL_CASES_INDEX[k])
                {
                    for (uint32_t l = 0; l < 5; l++)
                        if(LEVELS[j+1][SPECIAL_CASES[k][l]] == '#')
                            adjacent_bugs++;
                }
            }
            
            // If we foung bugs, change adjacent_bugs_levels to increase search range next iteration
            if (adjacent_bugs != 0)
                adjacent_bugs_level++;
            
            // Check if bugs die or appear
            if(LEVELS[j][i] == '.' && adjacent_bugs >= 1 && adjacent_bugs <= 2)
                new_map[j - (initial_min-1)][i] = '#';
            else if(LEVELS[j][i] == '#' && adjacent_bugs != 1) 
                new_map[j - (initial_min-1)][i] = '.';
            else
                new_map[j - (initial_min-1)][i] = LEVELS[j][i];
        }
        // if we generate a bug in a lever where there were none before, we increase depth check
        if(adjacent_bugs_level > 0 && j == initial_min-1)
            MIN_DEPTH--;

        if(adjacent_bugs_level > 0 && j == initial_max+1)
            MAX_DEPTH++;
    }
    
    // copy new map to current map
    for (uint32_t i = initial_min - 1; i <= initial_max + 1; i++)
    {
        for (uint32_t j = 0; j < 25; j++)
            LEVELS[i][j] = new_map[i - (initial_min-1)][j];
    }
}

void print_area(char* map)
{
    for (uint32_t i = 0; i < 25; i++)
    {
        if(i % GRID_WIDTH == 0)
            printf("\n%c", map[i]);
        else
            printf("%c", map[i]);
    }
    printf("\n");
}

int main(int argc, char** argv)
{
    cmd_input_validate(argc, argv, "filename", 2);
    uint32_t size = 0;
    char **lines = file_read_with_tokenizer(argv[1], '\n', &size);

    GRID_WIDTH = strlen(lines[0]);
    GRID_HEIGHT = size;

    char map[25];
    uint32_t counter = 0;
    for (uint32_t i = 0; i < GRID_HEIGHT; i++)
        for (uint32_t j = 0; j < GRID_WIDTH; j++)
            map[counter++] = lines[i][j];

    // Set initial state
    initialize_levels(map);
    MIN_DEPTH = MAX_DEPTH = START;
     for (uint32_t i = 0; i < DEPTH; i++)
        print_area(LEVELS[i]); 
   
    // Generate the map after specified STEPS
    for (uint32_t i = 0; i < STEPS; i++)
        generate_next_state();
    
    uint32_t result = count_bugs();
    printf("After %d steps there are %d bugs.\n", STEPS, result); // Output -> 1912

    // free resources
    free_char_pp(lines, size);

    return 0;
}