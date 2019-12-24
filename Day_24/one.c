#include <stdio.h>
#include <stdint.h>

#include "../lib/file.h" 
#include "../lib/utils.h" 
#include "../lib/vector.h" 

uint32_t POWERS_OF_TWO[25] = {1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768, 65536, 131072, 262144, 524288, 1048576, 2097152, 4194304, 8388608, 16777216}; //, 33554432};
uint32_t GRID_WIDTH, GRID_HEIGHT;

// modifies current map to next map and returns current state value
uint32_t get_next_state(char* current_map)
{
    uint32_t adjacent_bugs;
    uint32_t state_value = 0;
    char new_map[25];
    for (uint32_t i = 0; i < 25; i++)
    {
        adjacent_bugs = 0;
        // check bugs around current position
        if(i >= GRID_WIDTH) // we can check up
            if(current_map[i-GRID_WIDTH] == '#')
                adjacent_bugs++;

        if(i < GRID_WIDTH * (GRID_HEIGHT-1)) // we can check down
            if(current_map[i+GRID_WIDTH] == '#')
                adjacent_bugs++;

        if(i % GRID_WIDTH != 0) // we can check left
            if(current_map[i-1] == '#')
                adjacent_bugs++;

        if((i+1) % GRID_WIDTH != 0) // we can check right 
            if(current_map[i+1] == '#')
                adjacent_bugs++;

        // Check if bugs die or appear
        if(current_map[i] == '.' && adjacent_bugs >= 1 && adjacent_bugs <= 2)
            new_map[i] = '#';
        else if(current_map[i] == '#' && adjacent_bugs != 1) // Bug doesn't die 
            new_map[i] = '.';
        else
            new_map[i] = current_map[i];
        
        if (new_map[i] == '#')
            state_value += POWERS_OF_TWO[i];
    }

    // copy new map to current map
    for (uint32_t i = 0; i < 25; i++)
        current_map[i] = new_map[i];
    
    return state_value;
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
    printf("\n\n");
}

int main(int argc, char** argv)
{
    cmd_input_validate(argc, argv, "filename", 2);
    uint32_t size = 0;
    char **lines = file_read_with_tokenizer(argv[1], '\n', &size);

    GRID_WIDTH = strlen(lines[0]);
    GRID_HEIGHT = size;

    IntVector *states = iv_create_vector(10000);

    // Set initial state
    char map[25];
    uint32_t counter = 0;
    uint32_t state_value = 0;
    for (uint32_t i = 0; i < GRID_HEIGHT; i++)
    {
        for (uint32_t j = 0; j < GRID_WIDTH; j++)
        {
            if(lines[i][j] == '#') // if a bug, we add to state value
                state_value += POWERS_OF_TWO[i * GRID_WIDTH + j];
            map[counter++] = lines[i][j];
        }
    }
    iv_add_int(states, state_value); // add first state value

    
    uint32_t found = 0;
    while (found == 0)
    {
        state_value = get_next_state(map);
        if(iv_find(states, state_value) == 1)
            found = 1;
        else
            iv_add_int(states, state_value);
    }
    printf("Value found is %d \n", state_value); // Output -> 32513278

    // free resources
    free_char_pp(lines, size);

    return 0;
}