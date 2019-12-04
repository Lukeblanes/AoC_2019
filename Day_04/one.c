#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

const uint32_t min = 197487;
const uint32_t max = 673251;

uint32_t check_requirements(uint32_t num)
{
    char snum[7];
    snprintf(snum, 7, "%d", num);
    uint32_t return_val = 0;
    
    for(int i = 1; i < 6; ++i)
    {
        if(snum[i-1] > snum[i])
            return 0;
        if(snum[i-1] == snum[i])
            return_val = 1;
    }

    return return_val;
}

int main(int argc, char** argv)
{
    uint32_t matches = 0;

    for (uint32_t i = min; i <= max; ++i)
    {
        if (check_requirements(i) != 0)
            matches++;
    }

    printf("%d different passwords meet the criteria\n", matches);
    return 0;
}