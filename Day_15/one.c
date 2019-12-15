#include <stdio.h>
#include <stdint.h>

#include "../lib/vector.h"
#include "../lib/file.h"
#include "../lib/utils.h"

#define NUM_OPCODES 9
#define GENERATE_INTCODE_OUTPUT 0

// directions: north (1), south (2), west (3), and east (4)
uint32_t g_program_input = 0;

// Relative base used for relative mode
int64_t g_relative_base = 0;

// pc instruction to keep track of where we left off
int64_t g_PC = 0;

uint32_t g_pause_flag = 0;

uint32_t g_end_program = 0;

// used to track how many elements are in the output 
int32_t g_output_counter = 0;
// status codes: 
// - 0: The repair droid hit a wall. Its position has not changed.
// - 1: The repair droid has moved one step in the requested direction.
// - 2: The repair droid has moved one step in the requested direction 
//      and new position is the location of the oxygen system.
// ONLY g_output_elems[0] is used
int32_t g_output_elems[3];

coordinate MOVES[4] = {{0, -1}, {0, 1}, {-1, 0}, {1, 0}};
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
    g_output_counter = 0;
}

void print_grid(char* grid, uint32_t length, uint32_t smallest_x, uint32_t biggest_x)
{
    uint32_t empty_checker = 0;
    uint32_t line = 0;
    // length * length grid
    for (uint32_t i = 1; i < length*length + 1; i++)
    {
        if((i - 1) % length == 0)
        {
            empty_checker = 0;
            for(uint32_t j = line * length; j < line * length + length; j++)
                if(grid[j] != ' ')
                    empty_checker = 1; // not empty

            line++;
        }

        if(empty_checker == 1 && (i % length) > smallest_x - 3 && (i % length) < biggest_x + 3)
            printf("%c", grid[i-1]);

        if (i % length == 0 && empty_checker == 1)
            printf("\n");
    }
}

uint32_t get_new_dir(char* grid, uint32_t grid_width, coordinate pos, coordinate* robot_dir)
{
    uint32_t DIRS = 4;

    uint32_t dir[4];
    if(robot_dir->x == 0 && robot_dir->y == -1) { // UP
        dir[0] = 4; dir[1] = 1; dir[2] = 3; dir[3] = 2;
    } else if(robot_dir->x == 1 && robot_dir->y == 0) { // RIGHT
        dir[0] = 2; dir[1] = 4; dir[2] = 1; dir[3] = 3;
    } else if(robot_dir->x == 0 && robot_dir->y == 1) { // DOWN
        dir[0] = 3; dir[1] = 2; dir[2] = 4; dir[3] = 1;
    } else if(robot_dir->x == -1 && robot_dir->y == 0) { // LEFT
        dir[0] = 1; dir[1] = 3; dir[2] = 2; dir[3] = 4;
    }

    coordinate check;

    // unvisited have priority
    for (uint32_t i = 0; i < DIRS; i++)
    {
        check.x = pos.x + MOVES[dir[i]-1].x;
        check.y = pos.y + MOVES[dir[i]-1].y;
        if(grid[check.y * grid_width + check.x] == ' ')
        {
            robot_dir->x = MOVES[dir[i]-1].x;
            robot_dir->y = MOVES[dir[i]-1].y;
            return dir[i];
        }
    }

    // After unvisited, dotted have priority
    for (uint32_t i = 0; i < DIRS; i++)
    {
        check.x = pos.x + MOVES[dir[i]-1].x;
        check.y = pos.y + MOVES[dir[i]-1].y;
        if(grid[check.y * grid_width + check.x] == '.') 
        {
            robot_dir->x = MOVES[dir[i]-1].x;
            robot_dir->y = MOVES[dir[i]-1].y;
            return dir[i];
        }
    }

    printf("ERROR: shouldn't arrive here\n");
    exit(-1);
    return 1;
}

uint32_t get_min_steps(char* grid, uint32_t grid_width, coordinate previous, coordinate new)
{
    coordinate aux;
    uint32_t min_aux, min = 4000000;
    for (uint32_t i = 0; i < 4; i++)
    {
        min_aux = UINT32_MAX;
        aux.x = new.x + MOVES[i].x;
        aux.y = new.y + MOVES[i].y;

        // if not previous direction and a valid move
        if((!(aux.x == previous.x && aux.y == previous.y)) && grid[aux.y * grid_width + aux.x] != '#')
        {
            // search how many paths we can take in new position
            coordinate aux_count;
            uint32_t paths = 0;
            for (uint32_t i = 0; i < 4; i++)
            {
                aux_count.x = new.x + MOVES[i].x;
                aux_count.y = new.y + MOVES[i].y;
                if(!(aux.x == new.x && aux.y == new.y))
                    if(grid[aux_count.y * grid_width + aux_count.x] == '.')
                        paths++;
            }

            if(grid[aux.y * grid_width + aux.x] == 'o') // oxygen found
                min_aux = 1;
            else if(paths != 0)
                min_aux = 1 + get_min_steps(grid, grid_width, new, aux);
            else
                min_aux = 4000000;
        }
        if(min_aux < min)
            min = min_aux;
    }
    return min;
}

int main(int argc, char** argv)
{
    uint32_t size = 0;
    char **lines = file_read_with_tokenizer("input.txt", '\n', &size);

    uint32_t tokenCount = 0;
    char** tokens = tokenizer_with_delimiter(lines[0], ',', &tokenCount);

    // Store program
    IntVector *inputVector = iv_create_vector(10000);
    for(int i = 0; i < tokenCount; i++)
        iv_add_int(inputVector, atol(tokens[i]) );

    // create grid
    uint32_t grid_width = 1000;   
    uint32_t grid_size = grid_width * grid_width;   
    char *grid = malloc(sizeof(char*) * grid_size);

    // fill grid
    for (uint32_t i = 0; i < grid_size; i++)
        grid[i] = ' '; // black or unpainted
    
    // Initial robot position
    coordinate robot_starting_pos = {500, 500}; // around the middle initially
    coordinate robot_pos = { robot_starting_pos.x, robot_starting_pos.y }; 
    // Initial robot direction
    coordinate robot_dir = {0, -1}; // Initially heads up

    coordinate aux, oxygen;

    // hard exit just in case
    uint32_t iterations = 1;

    CoordinateVector *visited = cv_create_coordinate_vector(10000);
    cv_add_unique_coordinate(visited, robot_pos);
    uint32_t smallest_x = grid_width;
    uint32_t biggest_x = 0;
    g_program_input = 1; // initially move up
    while (g_end_program == 0 && iterations++ <= 100000)
    {
        run_intcode_program(inputVector->elements, g_PC, 0, 0, 0, 0); 
       
        if(g_output_elems[0] == 0)
        {
            // fill the wall to the position we would move
            aux.x = robot_pos.x + MOVES[g_program_input-1].x; 
            aux.y = robot_pos.y + MOVES[g_program_input-1].y; 
            grid[aux.y * grid_width + aux.x] = '#';
            g_program_input = get_new_dir(grid, grid_width, robot_pos, &robot_dir);
        } else if(g_output_elems[0] == 1){
            robot_pos.x += MOVES[g_program_input-1].x;
            robot_pos.y += MOVES[g_program_input-1].y;
            grid[robot_pos.y * grid_width + robot_pos.x] = '.';
            g_program_input = get_new_dir(grid, grid_width, robot_pos, &robot_dir);
        } else if(g_output_elems[0] == 2){
            robot_pos.x += MOVES[g_program_input-1].x;
            robot_pos.y += MOVES[g_program_input-1].y;
            oxygen.x = robot_pos.x;
            oxygen.y = robot_pos.y;
            grid[robot_pos.y * grid_width + robot_pos.x] = 'o';
            g_program_input = get_new_dir(grid, grid_width, robot_pos, &robot_dir);
        }

        if(robot_pos.x == 500 && robot_pos.y == 500 && iterations > 100)
        {
            printf("The whole map has been generated\n");
            break;
        }
 
        if(robot_pos.x < smallest_x) smallest_x = robot_pos.x;
        if(robot_pos.x > biggest_x) biggest_x = robot_pos.x;
    }

    printf("Oxygen is at (%d, %d)\n", oxygen.x, oxygen.y); // at pos (514, 488)
//    print_grid(grid, grid_width, smallest_x, biggest_x);

    coordinate first_next_step = { robot_starting_pos.x, robot_starting_pos.y -1 };
    uint32_t result = 1 + get_min_steps(grid, grid_width, robot_starting_pos, first_next_step);
 
    printf("Minimum steps from origin to oxygen is %d\n", result); // output -> 318
    // free resources
    free_char_pp(tokens, tokenCount);
    free_char_pp(lines, size);

    return 0;
}
