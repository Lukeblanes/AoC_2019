#include <stdio.h>
#include <stdint.h>

#include "../lib/file.h"

const uint32_t INPUT_SIZE = 100;

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
    uint32_t array[INPUT_SIZE];
    file_read_ints_to_array(argv[1], array);
    
    int fuel_requirements = 0;

    for(uint16_t i = 0; i < INPUT_SIZE; ++i)
    {
        uint32_t fuel_required = complete_fuel_requirements(array[i]);
        fuel_requirements += fuel_required;
    }

    printf("Fuel requirements is %d\n", fuel_requirements);

    return 0;
}