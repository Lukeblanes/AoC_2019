#include <stdio.h>
#include <stdint.h>

#include "../lib/file.h"
#include "../lib/utils.h"

int main(int argc, char** argv)
{
    cmd_input_validate(argc, argv, "filename", 2);
    uint32_t size = 0;
    char** elements= file_read_with_tokenizer(argv[1], '\n', &size);
    
    int fuel_requirements = 0;

    for(uint16_t i = 0; i < size; ++i)
        fuel_requirements += atoi(elements[i]) / 3 - 2;

    printf("Fuel requirements is %d\n", fuel_requirements); // Output -> 3287620

    free_char_pp(elements, size);
    return 0;
}