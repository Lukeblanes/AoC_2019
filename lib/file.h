#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

// NOTE: Make sure, the file has an \n in the last entry of the input file
// This is to avoid having to use EOF as a separator.
void file_read_ints_to_array(const char* filename, uint32_t *array)
{
    FILE *file = fopen(filename, "r");
    char ch, buffer[20];
    int index_array_setter = 0;

    if (file == NULL) {
        printf("Couldn't open the file.");
    } else {
        int buffer_position_setter = 0;
        while( (ch = fgetc(file)) != EOF)
        {
            if ( ch == '\n')
            {
                buffer[buffer_position_setter] = '\0';
                array[index_array_setter++] = atoi(buffer);
                buffer_position_setter = 0;
            } else {
                buffer[buffer_position_setter++] = ch;
            }
        }
    }
}