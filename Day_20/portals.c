typedef struct {
    uint32_t size;
    CoordinateVector *entries; // Internal portals - They go into a smaller maze and a lower level
    CoordinateVector *exits;   // External portals - The go into a bigger maze and a higher level
    char name[MAX_NUM_PORTALS][2];
} Portals;

int32_t get_index(Portals* p, char first, char second)
{
    for (uint32_t i = 0; i < p->size; i++)
        if(p->name[i][0] == first && p->name[i][1] == second)
            return i;
    
    return -1;
}

Portals* get_portals(char** grid, uint32_t width, uint32_t height)
{
    Portals *portals = malloc(sizeof *portals);
    portals->size = 0; // initially we have no portals
    portals->entries = cv_create_coordinate_vector(MAX_NUM_PORTALS);
    portals->exits = cv_create_coordinate_vector(MAX_NUM_PORTALS);
    for (uint32_t i = 0; i < MAX_NUM_PORTALS; i++)
    {
        cv_add_coordinate(portals->entries, (coordinate) {.x = 0, .y = 0});
        cv_add_coordinate(portals->exits, (coordinate) {.x = 0, .y = 0});
    }

    uint32_t aux_index = 0;
    for (uint32_t i = 2; i < height - 2; i++)
    {
        for (uint32_t j = 2; j < width - 2; j++)
        {
            if(grid[i][j] == '.')
            {
                for (size_t k = 0; k < 4; k++)
                {
                    char first  = grid[(i + DIRS[k].y)][(j + DIRS[k].x )];
                    char second = grid[(i + (DIRS[k].y * 2))][(j + (DIRS[k].x * 2))];
                    if(first != second && first >= 'A' && first <= 'Z') { // different to check that it's not AA or ZZ
                        // Set name
                        if( k == 0 || k == 3) { // if up or left
                            portals->name[portals->size][0] = second;
                            portals->name[portals->size][1] = first;
                        } else {
                            portals->name[portals->size][0] = first;
                            portals->name[portals->size][1] = second;
                        }
                        aux_index = get_index(portals, portals->name[portals->size][0], portals->name[portals->size][1]);

                        if(aux_index == -1) // if portal doesn't exist, we increment size
                        {
                            aux_index = portals->size++; // if new, current portals size is the index 
                        }

                        if( j == 2 || j == width - 3 || i == 2 || i == height- 3) { // portals that go up a level
                            portals->exits->coordinates[aux_index].x = j;
                            portals->exits->coordinates[aux_index].y = i;
                        } else {  // portals that go down a level
                            portals->entries->coordinates[aux_index].x = j;
                            portals->entries->coordinates[aux_index].y = i;
                        }

                        if(portals->size == MAX_NUM_PORTALS)
                        {
                            printf("Not enough size for portals. Increase MAX_NUM_PORTALS\n");
                            exit(-1);
                        }
                        
                    } else if(first == second && first == 'A') {
                        INITIAL_DIR.x = DIRS[k].x * (-1);
                        INITIAL_DIR.y = DIRS[k].y * (-1);
                        START_POS.x = j;
                        START_POS.y = i;
                    } else if(first == second && first == 'Z') {
                        END_POS.x = j;
                        END_POS.y = i;
                    }
                }
            }
        }
    }
    
    return portals;
}

void portals_print(Portals* portals)
{
    printf("The %d portals are: \n", portals->size);
    for (uint32_t i = 0; i < portals->size; i++)
        printf("%c%c - (%d, %d)<->(%d, %d)\n", portals->name[i][0], portals->name[i][1], portals->entries->coordinates[i].x, portals->entries->coordinates[i].y,
                                               portals->exits->coordinates[i].x, portals->exits->coordinates[i].y);
}