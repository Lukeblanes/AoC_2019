#include <stdio.h>
#include <stdint.h>

#include "../lib/file.h" 
#include "../lib/utils.h"
#include "../lib/tree.h"

int main(int argc, char** argv)
{
    uint32_t size = 0;
    char **lines = file_read_with_tokenizer("input.txt", '\n', &size);

    Tree *tree = tree_init();
    char **orbits; 
   
    for(uint32_t i = 0; i < size; ++i)
    {
        uint32_t orbit_size = 0;
        orbits = tokenizer_with_delimiter(lines[i], ')', &orbit_size);
        tree_add(&tree, orbits[0], orbits[1]);
        free_char_pp(orbits, orbit_size);
    }
    
    // output -> 186597
    printf("total number of orbits is %d\n", tree_get_orbits(tree));

    // free resources
    free_char_pp(lines, size);

    return 0;
}