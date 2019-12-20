#include <stdio.h>
#include <stdint.h>

#include "../lib/file.h" 
#include "../lib/utils.h" 
#include "../lib/vector.h" 

// Global vars
coordinate DIRS[] = {{0,-1}, {1,0}, {0,1}, {-1,0}};
#define MAX_STEPS 556 
uint32_t SHORTEST = MAX_STEPS;
#define MAX_NUM_PORTALS 30

coordinate START_POS, END_POS; // set when calling get_portals
coordinate INITIAL_DIR;

#include "portals.c"

Portals *PORTALS;

char **GRID;
uint32_t GRID_WIDTH, GRID_HEIGHT;

// Not valid when:
// - we move to a place which isn't a wall or a portal
// - previous and new are the same coordinate
uint32_t check_new_pos(coordinate *previous, coordinate *current, coordinate* new)
{
    char currentChar = GRID[new->y][new->x];
    if(currentChar != '.') // if wall or a letter from a portal, wrong.
        return 0;

    if(previous->x == new->x && previous->y == new->y) // previous position
        return 0;

    return 1;
}

// searchs if current coordinate is a portal, 
// if it is a portal, returns the other side of the portal
coordinate check_teleportation(coordinate *new)
{
    for (uint32_t i = 0; i < PORTALS->size; i++)
    {
        if( PORTALS->entries->coordinates[i].x == new->x && PORTALS->entries->coordinates[i].y == new->y )
            return (coordinate) {.x = PORTALS->exits->coordinates[i].x, .y = PORTALS->exits->coordinates[i].y};
        else if( PORTALS->exits->coordinates[i].x == new->x && PORTALS->exits->coordinates[i].y == new->y )
            return (coordinate) {.x = PORTALS->entries->coordinates[i].x, .y = PORTALS->entries->coordinates[i].y};
    }
    return (coordinate) {-1, -1};
}

// Next step we would take after going through a portal
coordinate next_position_from_portal(coordinate *portal_pos)
{
    for (uint32_t i = 0; i < 4; i++)
        if(GRID[portal_pos->y + DIRS[i].y][portal_pos->x + DIRS[i].x] == '.') // 
            return (coordinate) {.x = portal_pos->x + DIRS[i].x, .y = portal_pos->y + DIRS[i].y};

    // If we reach here we exit
    printf("ERROR: HAVEN'T FOUND A NEXT POSITION FROM PORTAL\n");
    exit(-1);
}

uint32_t shortest_path(coordinate *previous, coordinate *current, uint32_t steps)
{
    uint32_t minimum = MAX_STEPS;
    if(steps >= MAX_STEPS) {
        return steps;
    } else if(current->x == END_POS.x && current->y == END_POS.y) {
        return steps;
    } else {
        coordinate newPos;
        coordinate teleportation_check;
        uint32_t aux;
        for (uint32_t i = 0; i < 4; i++)
        {
            newPos = (coordinate) { .x = current->x + DIRS[i].x, .y = current->y + DIRS[i].y };
            if(check_new_pos(previous, current, &newPos) == 1)
            {
                teleportation_check = check_teleportation(&newPos);
                if(teleportation_check.x == -1 && teleportation_check.y == -1) { // no teleporting
                    aux = shortest_path(current, &newPos, steps + 1);
                } else { // teleporting so previous is now other side of portal and new is the next position from portal 
                    newPos = next_position_from_portal(&teleportation_check);
                    aux = shortest_path(&teleportation_check, &newPos, steps + 3);
                }

                // store best amount of steps
                if (aux < minimum)
                    minimum = aux;
            }
        }
    }
    return minimum;
}

int main(int argc, char** argv)
{
    cmd_input_validate(argc, argv, "filename", 2);
    uint32_t size = 0;
    GRID = file_read_with_tokenizer(argv[1], '\n', &size);

    uint32_t GRID_WIDTH= strlen(GRID[0]);
    uint32_t GRID_HEIGHT = size;

    printf("Getting portals\n");
    PORTALS = get_portals(GRID, GRID_WIDTH, GRID_HEIGHT);

    printf("Starting pos (%d, %d) with initial dir(%d, %d) - Ending pos (%d, %d)\n", START_POS.x, START_POS.y, INITIAL_DIR.x, INITIAL_DIR.y, END_POS.x, END_POS.y);

    portals_print(PORTALS);

    uint32_t result = shortest_path(&START_POS, &((coordinate) {.x = START_POS.x + INITIAL_DIR.x, .y = START_POS.y + INITIAL_DIR.y}), 1); // First move will always be in one direction
    printf("The shortest path from AA to ZZ is %d\n", result); // Output -> 516

    // free resources
    free_char_pp(GRID, size);

    return 0;
}