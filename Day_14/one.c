#include <stdio.h>
#include <stdint.h>

#include "../lib/file.h" 
#include "../lib/utils.h" 
#include "../lib/dictionary.h" 
#include "../lib/vector.h" 

// dic is going to keep track of all the different generated vals and the ore used
void get_mins(char*** tokens, uint32_t* tokens_size, uint32_t line_size, Dictionary* dic, KeyVal keyval)
{
    // Find index where we can generate keyval.value
    uint32_t to_generate_pos = 0;
    for (int64_t i = 0; i < line_size; i++)
    {
        if(strcmp (tokens[i][tokens_size[i]-1], keyval.key) == 0 )
        {
            to_generate_pos = i;
            break;
        }
    }
       
    int64_t initial_val = dic_lookup(dic, keyval.key);
    int64_t current_val = initial_val;
    if(current_val == DIC_NOT_FOUND)
    {
        dic_add(dic, keyval.key, 0);
        current_val = initial_val = 0;
    }

    if(current_val >= keyval.value) { // we can consume from reserve
        dic_set(dic, keyval.key, current_val - keyval.value); 
    } else if(strcmp (tokens[to_generate_pos][1], "ORE") == 0 ) { // we can add to reserve
        dic_add(dic, "ORE", (atol(tokens[to_generate_pos][0])));
        dic_add(dic, keyval.key, atol(tokens[to_generate_pos][2] ));
        get_mins(tokens, tokens_size, line_size, dic, keyval);
    } else {
        // current_val will always be smaller than keyval.value

        int64_t to_create = keyval.value - current_val; // the amount to generate - the ones we already have
        int64_t counter;
        current_val = atol(tokens[to_generate_pos][tokens_size[to_generate_pos]-2]); // new val
        counter = to_create / current_val;
        if(to_create % current_val != 0) // we need excess values
            counter++;

        // We store the leftovers of creating the amount we need to complete the current key
        int64_t to_store = counter * atol(tokens[to_generate_pos][tokens_size[to_generate_pos]-2]) - to_create; 

        for (uint32_t i = 0; i < tokens_size[to_generate_pos]-2; i+=2)
        {
            KeyVal aux = {atol(tokens[to_generate_pos][i]) * counter, tokens[to_generate_pos][i+1]};
            printf("For key: {%s, %ld} -> created key: {%s, %ld}\n", keyval.key, keyval.value, aux.key, aux.value);
            get_mins(tokens, tokens_size, line_size, dic, aux);
        }

        // Add leftovers of current key
        dic_set(dic, keyval.key, to_store);
    }
}

int32_t get_min_fuel(char*** tokens, uint32_t* tokens_size, uint32_t line_size)
{
    uint32_t fuel_line;
    // Fuel is only in one line
    for (uint32_t i = 0; i < line_size; i++)
        if(strcmp (tokens[i][tokens_size[i]-1], "FUEL") == 0 )
            fuel_line = i;
    
    // Dictionary where the elements we need to generate are
    Dictionary* dic_elems_to_create = dic_create(10000);
    Dictionary* dic_state = dic_create(10000);
    for (uint32_t i = 0; i < tokens_size[fuel_line] - 2; i+=2)
    {
        dic_add(dic_state, tokens[fuel_line][i+1], 0);
        dic_add(dic_elems_to_create, tokens[fuel_line][i+1], atol(tokens[fuel_line][i]));
    }

    if(dic_elems_to_create->size != dic_state->size)
    {
        printf("Error: mismatch in dictionaries\n");
        exit(-1);
    }

    for (uint32_t i = 0; i < dic_elems_to_create->size; i++)
        get_mins(tokens, tokens_size, line_size, dic_state, dic_elems_to_create->elements[i]);

    int32_t max_fuel = dic_lookup(dic_state, "ORE");
    return max_fuel;
}

int main(int argc, char** argv)
{
    cmd_input_validate(argc, argv, "filename", 2);
    uint32_t size = 0;
    char** lines = file_read_with_tokenizer(argv[1], '\n', &size);

    char*** all_tokens = malloc(sizeof(char**) * size);
    uint32_t all_tokens_size[size];
        
    uint32_t tokenCount;
    for (uint32_t i = 0; i < size; i++)
    {
        tokenCount = 0;
        all_tokens[i] = tokenizer_with_delimiters(lines[i], " ,=>", &tokenCount);
        all_tokens_size[i] = tokenCount;
    }
    
    int32_t result = get_min_fuel(all_tokens, all_tokens_size, size);
    printf("max fuel used is %d\n", result); // Output -> 1046184

    // free resources
    free_char_pp(lines, size);

    return 0;
}
