#include <stdio.h>
#include <stdint.h>

#include "../lib/file.h" 
#include "../lib/utils.h" 
#include "../lib/vector.h" 

int main(int argc, char** argv)
{
    cmd_input_validate(argc, argv, "filename", 2);
    uint32_t size = 0;
    char **lines = file_read_with_tokenizer(argv[1], '\n', &size);

    uint32_t num_elems = strlen(lines[0]);

    IntVector *input_signal = iv_create_vector(num_elems);
    for (uint32_t i = 0; i < num_elems; i++)
        iv_add_int(input_signal, lines[0][i] - '0');

    int32_t base_pattern[4] = {0, 1, 0, -1};

    // precompute patterns
    int32_t full_pattern[num_elems][num_elems];
    IntVector *aux = iv_create_vector(num_elems * 2);
    for (uint32_t i = 0; i < num_elems; i++)
    {
        // create patter for corresponding iteration
        while (aux->size < num_elems + 1) 
            for(uint32_t l = 0; l < 4; l++)
                for (uint32_t k = 0; k < i+1; k++)
                    iv_add_int(aux, base_pattern[l]);
        
        // add it to matrix with correct offset
        for (uint32_t j = 0; j < num_elems; j++)
        {
            int64_t* elems = aux->elements;
            full_pattern[i][j] = elems[j+1]; 
        }
        iv_clear(aux);
    }

   // main loop...
   iv_clear(aux); // aux will be used to store results and at the end we switch
   int32_t counter;
   for (uint32_t i = 0; i < 100; i++)
   {
       for (uint32_t j = 0; j < num_elems; j++)
       {
            counter = 0;
            for (uint32_t k = 0; k < num_elems; k++)
                counter += input_signal->elements[k] * full_pattern[j][k];

           if(counter < 0) 
                counter *= -1;
            counter %= 10;
            iv_add_int(aux, counter);
       }
       iv_copy(input_signal, aux);
       iv_clear(aux);
   }
   
    // print first 8 chars
    for (size_t i = 0; i < 8; i++)
    {
        printf("%ld", input_signal->elements[i]); // Output -> 68764632
    }
    printf("\n");

    // free resources
    free_char_pp(lines, size);

    return 0;
}