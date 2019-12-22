#include <stdio.h>
#include <stdint.h>

#include "../lib/vector.h"
#include "../lib/file.h"
#include "../lib/utils.h"

#define DECK_SIZE 10007
#define POSITION 2019

void deal_into_new_stack(IntVector *deck)
{
    uint32_t aux;
    for (uint32_t i = 0; i < (deck->size / 2); i++)
    {
        aux = deck->elements[i];
        deck->elements[i] = deck->elements[deck->size-i-1];
        deck->elements[deck->size-i-1] = aux;
    }
}

void deal_cut_N_cards(IntVector* deck, int32_t n)
{
    IntVector *aux = iv_create_vector(deck->size + 1);
    if(n > 0) {
        for (uint32_t i = n; i < deck->size; i++)
            iv_add_int(aux, deck->elements[i]);

        for (uint32_t i = 0; i < n; i++)
            iv_add_int(aux, deck->elements[i]);
    } else{
        // we add since n is negative
        for (uint32_t i = deck->size + n; i < deck->size; i++)
            iv_add_int(aux, deck->elements[i]);

        for (uint32_t i = 0; i < deck->size + n; i++)
            iv_add_int(aux, deck->elements[i]);
    }
    iv_copy(deck, aux);
    iv_destroy_vector(aux);
}

void deal_with_increment_N(IntVector* deck, uint32_t n)
{
    IntVector *aux = iv_create_vector(deck->size + 1);
    iv_copy(aux, deck); // this already sets index 0 correctly so we skip one iteration

    uint32_t tracker = 0;
    // according to problem there are no collisions
    for (uint32_t i = 1; i < deck->size; i++)
    {
        tracker += n;
        aux->elements[tracker % deck->size] = deck->elements[i];
    }
    iv_copy(deck, aux);
    iv_destroy_vector(aux);
}

int main()
{
    uint32_t size = 0;
    char **lines = file_read_with_tokenizer("input.txt", '\n', &size);

    // Create deck
    IntVector *deck = iv_create_vector(DECK_SIZE + 1);
    for (uint32_t i = 0; i < DECK_SIZE; i++)
    {
        iv_add_int(deck, i);
    }

    // Perform different shuffles
    char** auxTokens;
    uint32_t numTokens = 0;
    for (uint32_t i = 0; i < size; i++)
    {
        auxTokens = tokenizer_with_delimiters(lines[i], " ", &numTokens);
        if(strcmp(auxTokens[0], "cut") == 0)
            deal_cut_N_cards(deck, atoi(auxTokens[1]));
        else if(strcmp(auxTokens[3], "stack") == 0)
            deal_into_new_stack(deck);
        else
            deal_with_increment_N(deck, atoi(auxTokens[3]));

        for (uint32_t i = 0; i < numTokens; i++)
            free(auxTokens[i]);
        free(auxTokens);
    }

    // Get index of 2019
    uint32_t result;
    for (uint32_t i = 0; i < deck->size; i++)
    {
        if(deck->elements[i] == POSITION)
        {
            result = i;
            break;
        }
    }
    printf("Deck at index 2019 is %d\n", result); // Output -> 8379

    free(deck);

    return 0;
}

 