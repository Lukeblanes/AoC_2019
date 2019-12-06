#include <stdio.h>
#include <stdint.h>

#include "../lib/file.h" 
#include "../lib/utils.h"
#include "../lib/tree.h"

#define INPUT_LINES 1294

int main(int argc, char** argv)
{
    char **lines = malloc(sizeof(char*) * INPUT_LINES);
    file_read_lines("input.txt", lines);

    char **orbits = malloc(sizeof(char*) * 2);

    Tree *tree = tree_init();
    tokenizer_with_splitter(lines[0], ')', orbits);
   
    for(uint32_t i = 0; i < INPUT_LINES; ++i)
    {
        tokenizer_with_splitter(lines[i], ')', orbits);
        tree_add(&tree, orbits[0], orbits[1]);
    }
    
    printf("total number of orbits is %d\n", tree_get_orbits(tree));
    // output -> 186597

    // free resources
    free_char_pp(lines, INPUT_LINES);
    free_char_pp(orbits, 2);

    return 0;
}