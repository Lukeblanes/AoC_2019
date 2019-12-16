#include "../lib/coordinate.h"

#include <stdio.h>
#include <stdlib.h>

#define DEFAULT_CAPACITY 8

typedef struct {
    coordinate* coordinates;
    int32_t capacity;
    int32_t size;
} CoordinateVector;

typedef struct {
    coordinate3d* coordinates;
    int32_t capacity;
    int32_t size;
} Coordinate3DVector;

// boolean
// should be renamed to find coordinate
int cv_check_coordinate(CoordinateVector* cv, coordinate c)
{
    for(int i = 0; i < cv->size; i++)
    {
        if (cv->coordinates[i].x == c.x && cv->coordinates[i].y == c.y)
            return 1;
    }
    return 0;
}

CoordinateVector* cv_create_coordinate_vector(int capacity)
{
    CoordinateVector *created = (CoordinateVector*)malloc(sizeof *created);
    created->coordinates = (coordinate*)malloc(((sizeof (int32_t)) * 2) * capacity);
    created->capacity = capacity;
    created->size = 0;
    return created;
}

Coordinate3DVector* cv3_create_coordinate_vector(int capacity)
{
    Coordinate3DVector *created = (Coordinate3DVector*)malloc(sizeof *created);
    created->coordinates = (coordinate3d*)malloc(((sizeof (int32_t)) * 3) * capacity);
    created->capacity = capacity;
    created->size = 0;
    return created;
}

void cv_print(const CoordinateVector* cv)
{
    printf("Vector of capacity %d and size %d has elements:\n", cv->capacity, cv->size);
    for(int i = 0; i < cv->size; ++i)
    {
        printf("(%d, %d) ", cv->coordinates[i].x, cv->coordinates[i].y);
    }
    printf("\n");
}

void cv3_print(const Coordinate3DVector* cv)
{
    printf("Vector of capacity %d and size %d has elements:\n", cv->capacity, cv->size);
    for(int i = 0; i < cv->size; ++i)
    {
        printf("(%d, %d, %d) ", cv->coordinates[i].x, cv->coordinates[i].y, cv->coordinates[i].z);
    }
    printf("\n");
}

void cv_add_coordinate(CoordinateVector* cv, coordinate c)
{
    cv->coordinates[cv->size++] = c;

    if (cv->capacity == cv->size) // realloc in this case
    { 
        int32_t capacity_new = cv->capacity * cv->capacity;
        coordinate* aux = (coordinate*)malloc(((sizeof (int32_t)) * 2) * capacity_new);

        for(int i = 0; i < cv->capacity; ++i)
        {
            aux[i] = cv->coordinates[i];
        }
        free(cv->coordinates);
        cv->coordinates = aux;
        cv->capacity = capacity_new;
    }
}

// we search to see if it exists before adding
uint32_t cv_add_unique_coordinate(CoordinateVector* cv, coordinate c)
{
    if(cv_check_coordinate(cv, c) != 1) // it doesn't exist so we create
    {
        cv_add_coordinate(cv, c);
        return 1;
    }
    return 0;
}

void cv3_add_coordinate(Coordinate3DVector* cv, coordinate3d c)
{
    cv->coordinates[cv->size++] = c;

    if (cv->capacity == cv->size) // realloc in this case
    { 
        int32_t capacity_new = cv->capacity * cv->capacity;
        coordinate3d* aux = (coordinate3d*)malloc(((sizeof (int32_t)) * 3) * capacity_new);

        for(int i = 0; i < cv->capacity; ++i)
        {
            aux[i] = cv->coordinates[i];
        }
        free(cv->coordinates);
        cv->coordinates = aux;
        cv->capacity = capacity_new;
    }
}

void cv_clear(CoordinateVector* cv)
{
    cv->size = 0;
}

void cv3_clear(Coordinate3DVector* cv)
{
    cv->size = 0;
}

int cv_get_pos(CoordinateVector* cv, coordinate c)
{
    for(int i = 0; i < cv->size; i++)
    {
        if (cv->coordinates[i].x == c.x && cv->coordinates[i].y == c.y)
            return i;
    }
    return 0;
}

typedef struct {
    int64_t* elements;
    int64_t capacity;
    int64_t size;
} IntVector;

IntVector* iv_create_vector(int capacity)
{
    IntVector *created = malloc(sizeof *created);
    created->elements = malloc((sizeof(int64_t)) * capacity);
    created->capacity = capacity;
    created->size = 0;
    return created;
}

void iv_add_int(IntVector* iv, int64_t to_add)
{
    iv->elements[iv->size++] = to_add;

    if (iv->capacity == iv->size) // realloc in this case
    { 
        printf("Resized IntVector since there isn't enough space\n");
        int64_t capacity_new = iv->capacity * 2;
        int64_t* aux = malloc((sizeof(int64_t)) * capacity_new);

        for(int i = 0; i < iv->capacity; ++i)
        {
            aux[i] = iv->elements[i];
        }
        free(iv->elements);
        iv->elements = aux;
        iv->capacity = capacity_new;
    }
}

void iv_clear(IntVector* iv)
{
    iv->size = 0;
}

// destroys iv_dest
void iv_copy(IntVector* dest, IntVector* source)
{
    iv_clear(dest);
    for (uint32_t i = 0; i < source->size; i++)
        iv_add_int(dest, source->elements[i]);
}

