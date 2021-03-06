#include <stdio.h>
#include <stdint.h>

#include "../lib/file.h" 
#include "../lib/utils.h" 

#define IMAGE_WIDTH 25
#define IMAGE_HEIGHT 6
#define INPUT_LENGTH 15000

// number of 1 digits multiplied by number of 2 digits in layer with least zeros
uint32_t calculate_checksum(char* input, uint32_t length, uint32_t im_width, uint32_t im_height)
{
    uint32_t layer_size = im_width * im_height;
    uint32_t layers = length / layer_size;
    uint32_t zero_count, one_count, two_count;
    uint32_t least_zeros = 10000000;
    uint32_t least_calculation = 0;
    for(int32_t i = 0; i < layers; ++i)
    {
        zero_count = one_count = two_count = 0;

        for(int32_t j = 0; j < layer_size; j++)
        {
            if (input[i*layer_size+j] == '0')
                zero_count++;
            if (input[i*layer_size+j] == '1')
                one_count++;
            if (input[i*layer_size+j] == '2')
                two_count++;
        }

        if(zero_count < least_zeros)
        {
            least_zeros = zero_count;
            least_calculation = two_count * one_count;
        }
    }
    return least_calculation;
}

int main(int argc, char** argv)
{
    uint32_t size = 0;
    char **lines = file_read_with_tokenizer("input.txt", '\n', &size);

    uint32_t result = calculate_checksum(lines[0], INPUT_LENGTH, IMAGE_WIDTH, IMAGE_HEIGHT);

    printf("The calculation is %d\n", result); // output -> 2806

    // free resources
    free_char_pp(lines, size);

    return 0;
}