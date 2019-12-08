#include <stdio.h>
#include <stdint.h>

#include "../lib/file.h" 
#include "../lib/utils.h" 

#define INPUT_LINES 1
#define IMAGE_WIDTH 25
#define IMAGE_HEIGHT 6
#define INPUT_LENGTH 15000

void draw_image(char* input, uint32_t length, uint32_t im_width, uint32_t im_height)
{
    uint32_t layer_size = im_width * im_height;
    uint32_t layers = length / layer_size;

    char image[layer_size]; // initially transparent
    for(uint32_t i = 0; i < layer_size; i++)
        image[i] = '2';

    for(int32_t i = 0; i < layers; ++i)
    {
        for(int32_t j = 0; j < layer_size; j++)
        {
            if(image[j] == '2')
                image[j] = input[i*layer_size+j];
        }
    }

    printf("Resulting image is: \n");
    for(int32_t j = 0; j < layer_size; j++)
    {
        if((j+1)%im_width == 0)
            printf("%c\n", image[j]);
        else
            printf("%c", image[j]);
    }
    printf("\n");
}

int main(int argc, char** argv)
{
    char **lines = malloc(sizeof(char*) * INPUT_LINES);
    file_read_lines("input.txt", lines);

    draw_image(lines[0], INPUT_LENGTH, IMAGE_WIDTH, IMAGE_HEIGHT); // message: ZBJAB

    // free resources
    free_char_pp(lines, INPUT_LINES);

    return 0;
}