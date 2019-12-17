#include <stdio.h>
#include <stdint.h>

#include "../lib/file.h" 
#include "../lib/utils.h" 
#include "../lib/vector.h" 

#define AUGMENT 10000

// Since total input_size is 650 * 10000 = 6500000
// Our index, is more than half meaning that we don't need to calculate the pattern 
// It will always be 1 from 5977269 until the end
int main(int argc, char** argv)
{
    cmd_input_validate(argc, argv, "filename", 2);
    uint32_t size = 0;
    char **lines = file_read_with_tokenizer(argv[1], '\n', &size);

    // number in original input
    uint32_t num_elems = strlen(lines[0]);
    char message_offset_aux[8];
    // store first 7 digits
    for (size_t i = 0; i < 7; i++)
        message_offset_aux [i] = lines[0][i];
        
    message_offset_aux[7] = '\0';
    uint32_t message_offset = atoi(message_offset_aux);
    printf("Message offset is %d\n", message_offset);

    IntVector *input_signal = iv_create_vector((num_elems * AUGMENT) - message_offset);
    IntVector *aux = iv_create_vector((num_elems * AUGMENT) - message_offset); // used to store
    for (uint32_t i = message_offset; i < num_elems * AUGMENT; i++)
        iv_add_int(input_signal, lines[0][i % num_elems] - '0');

    // main loop...
    int32_t counter;
    // Improvable solution, left it running overnight
    for (uint32_t i = 0; i < 100; i++)
    {
        printf("Phase %d\n", i);
        for (uint32_t j = message_offset; j < num_elems * AUGMENT; j++)
        {
            counter = 0;
            for (uint32_t k = j; k < num_elems * AUGMENT; k++)
                counter += input_signal->elements[k-message_offset];

            if(counter < 0) 
                counter *= -1;
            counter %= 10;

            iv_add_int(aux, counter);
        }
        iv_copy(input_signal, aux);
        iv_clear(aux);
    }

    // print first 8 chars
    printf("First 8 digits of input_signal with offset %d: ", message_offset);
    for (uint32_t i = 0; i < 8; i++)
        printf("%ld", input_signal->elements[i]); // Output -> 52825021
    printf("\n");

    // free resources
    free_char_pp(lines, size);

    return 0;
}