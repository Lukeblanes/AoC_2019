#include "../lib/coordinate.h"

#include <stdio.h>
#include <stdlib.h>

#define DEFAULT_CAPACITY 8

typedef struct {
    coordinate* coordinates;
    int32_t capacity;
    int32_t size;
} CoordinateVector;

CoordinateVector* cv_create_coordinate_vector(int capacity)
{
    CoordinateVector *created = (CoordinateVector*)malloc(sizeof *created);
    created->coordinates = (coordinate*)malloc(((sizeof (int32_t)) * 2) * capacity);
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

// boolean
int cv_check_coordinate(CoordinateVector* cv, coordinate c)
{
    for(int i = 0; i < cv->size; i++)
    {
        if (cv->coordinates[i].x == c.x && cv->coordinates[i].y == c.y)
            return 1;
    }
    return 0;
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
