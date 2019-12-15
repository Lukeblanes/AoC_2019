#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define DEFAULT_CAPACITY 8
#define DIC_NOT_FOUND INT64_MIN

typedef struct {
    int64_t value;
    char* key;
} KeyVal;

typedef struct {
    int32_t capacity;
    int32_t size;
    KeyVal* elements;
} Dictionary;


// Implementation of dictionary improvable
Dictionary* dic_create(int capacity)
{
    Dictionary *created = (Dictionary*)malloc(sizeof *created);
    created->capacity = capacity;
    created->size = 0;
    created->elements = (KeyVal*)malloc( (sizeof (KeyVal)) * capacity);
    return created;
}

// if element already exists, just add value to current value
void dic_add(Dictionary* dic, const char* key, int64_t value)
{
    // Search and modify if it already exists
    for(uint32_t i = 0; i < dic->size; i++)
    {
        if(strcmp(dic->elements[i].key, key) == 0)
        {
            dic->elements[i].value += value;
            return;
        }
    }

    // If not in dictionary,
    // check if there's enough space
    // TODO: ATM just use high num
    if(dic->size == dic->capacity)
    {
        printf("Increase number of dic, realloc not implemented yet\n");
    }

    dic->elements[dic->size].value = value;
    dic->elements[dic->size].key = (char*)malloc(strlen(key) + 1);
    strcpy(dic->elements[dic->size].key, key);

    dic->size++;
}

// if element already exists, just add value to current value
void dic_set(Dictionary* dic, const char* key, int64_t value)
{
    // Search and modify if it already exists
    for(uint32_t i = 0; i < dic->size; i++)
    {
        if(strcmp(dic->elements[i].key, key) == 0)
        {
            dic->elements[i].value = value;
            return;
        }
    }
    printf("We haven't found the dic to set");
    return;

}
// TODO: 
// void dic_remove(const char* key)
// void dic_modifiy(Dictionary* dic, const char* key, int64_t modification)

int64_t dic_lookup(Dictionary* dic, const char* key)
{
    // Search and modify if it already exists
    for(uint32_t i = 0; i < dic->size; i++)
        if(strcmp(dic->elements[i].key, key) == 0)
            return dic->elements[i].value;

    return DIC_NOT_FOUND;
}

void dic_print(const Dictionary* dic)
{
    printf("Dictionary of capacity %d and size %d has elements:\n", dic->capacity, dic->size);
    for(int i = 0; i < dic->size; ++i)
    {
        printf("{%s, %ld} ", dic->elements[i].key, dic->elements[i].value);
    }
    printf("\n");
}
