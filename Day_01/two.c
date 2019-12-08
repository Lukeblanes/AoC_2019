#include <stdio.h>
#include <stdint.h>

#include "../lib/file.h"
#include "../lib/utils.h"

uint32_t complete_fuel_requirements(uint32_t mass)
{
    int32_t fuel_calculation;
    if ( (fuel_calculation = mass / 3 - 2) < 0 )
        return 0;
    else
        return fuel_calculation + complete_fuel_requirements(fuel_calculation);
}

int main(int argc, char** argv)
{
    cmd_input_validate(argc, argv, "filename", 2);
    uint32_t size = 0;
    char** elements = file_read_with_tokenizer(argv[1], '\n', &size);
 
    int fuel_requirements = 0;

    for(uint16_t i = 0; i < size; ++i)
    {
        uint32_t fuel_required = complete_fuel_requirements(atoi(elements[i]));
        fuel_requirements += fuel_required;
    }

    printf("Fuel requirements is %d\n", fuel_requirements); // Output -> 4928567

    free_char_pp(elements, size);
    return 0;
}