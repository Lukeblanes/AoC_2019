#include <stdio.h>
#include <stdint.h>

#include "../lib/file.h"

const uint32_t INPUT_SIZE = 100;

int main(int argc, char** argv)
{
    uint32_t array[INPUT_SIZE];
    file_read_int_to_array(argv[1], array);
    
    int fuel_requirements = 0;

    for(uint16_t i = 0; i < INPUT_SIZE; ++i)
    {
        fuel_requirements += array[i] / 3 - 2;
    }

    printf("Fuel requirements is %d\n", fuel_requirements);

    return 0;
}