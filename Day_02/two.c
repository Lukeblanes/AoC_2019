#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

const uint32_t STEP_SIZE = 4;
const uint32_t INPUT_SIZE = 133;
const uint32_t input[] = { 1,12,2,3,1,1,2,3,1,3,4,3,1,5,0,3,2,13,1,19,1,19,10,23,2,10,23,27,1,27,6,31,1,13,31,35,1,13,35,39,1,39,10,43,2,43,13,47,1,47,9,51,2,51,13,55,1,5,55,59,2,59,9,63,1,13,63,67,2,13,67,71,1,71,5,75,2,75,13,79,1,79,6,83,1,83,5,87,2,87,6,91,1,5,91,95,1,95,13,99,2,99,6,103,1,5,103,107,1,107,9,111,2,6,111,115,1,5,115,119,1,119,2,123,1,6,123,0,99,2,14,0,0 };

const uint32_t OUTPUT = 19690720;

void reset_array(const uint32_t* original, uint32_t* to_reset)
{
    for(int i = 0; i < INPUT_SIZE; ++i)
        to_reset[i] = original[i];
}

void print_array(const uint32_t* array)
{
    printf("(");
    for(uint32_t i = 0; i < INPUT_SIZE; i++)
    {
        printf("%d, ", array[i]);
    }
    printf(")\n");
}

void program_bruteforce_output(uint32_t* array, uint32_t input1, uint32_t input2)
{
    array[1] = input1;
    array[2] = input2;

    uint32_t a, b;
    for(uint16_t i = 0; i < INPUT_SIZE; i += STEP_SIZE)
    {
        if(array[i] == 99) // Halt
            break;
        else 
        {    
            a = array[array[i+1]];
            b = array[array[i+2]];

            if(array[i] == 1) // Add
            {
                array[array[i+3]] = a + b;
            }

            if(array[i] == 2) // Mul
            {
                array[array[i+3]] = a * b;
            }
        }
    }

    if(array[0] == OUTPUT)
    {
        printf("With input1 = %d and input2 = %d, we get the output %d\n", input1, input2, OUTPUT);
        exit(0);
    }
}

int main(int argc, char** argv)
{
    uint32_t aux[INPUT_SIZE];
    reset_array(input, aux);

    for(uint32_t i = 0; i <= 99; i++)
    {
        for(uint32_t j = 0; j <= 99; j++)
        {
            program_bruteforce_output(aux, i, j);
            reset_array(input, aux);
        }
    }

    return 0;
}
