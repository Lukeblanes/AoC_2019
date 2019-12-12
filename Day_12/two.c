#include <stdio.h>
#include <stdint.h>

#include "../lib/file.h" 
#include "../lib/utils.h" 
#include "../lib/vector.h" 

void step(Coordinate3DVector* moons, Coordinate3DVector* velocities)
{
    // Calculate velocities
    for(uint32_t i = 0; i < 4; i++) // Moon we are looking at
    {
        for(uint32_t j = 0; j < 4; j++) // Moons we are comparing with
        { 
            if(i != j) // check different moons than current
            {
                if(moons->coordinates[i].x < moons->coordinates[j].x)
                    velocities->coordinates[i].x += 1;
                else if(moons->coordinates[i].x > moons->coordinates[j].x)
                    velocities->coordinates[i].x -= 1;
                
                if(moons->coordinates[i].y < moons->coordinates[j].y)
                    velocities->coordinates[i].y += 1;
                else if(moons->coordinates[i].y > moons->coordinates[j].y)
                    velocities->coordinates[i].y -= 1;

                if(moons->coordinates[i].z < moons->coordinates[j].z)
                    velocities->coordinates[i].z += 1;
                else if(moons->coordinates[i].z > moons->coordinates[j].z)
                    velocities->coordinates[i].z -= 1;
            }
        }
    }

    // Once we have velocities, modify position
    for(uint32_t i = 0; i < 4; i++) 
    {
        moons->coordinates[i].x += velocities->coordinates[i].x;
        moons->coordinates[i].y += velocities->coordinates[i].y;
        moons->coordinates[i].z += velocities->coordinates[i].z;
    }
}

uint32_t check_state(Coordinate3DVector* coords, int32_t* original_z, uint32_t offset)
{
    int32_t current;
    for (uint32_t i = 0; i < 4; i++)
    {
        // we add i to the coordinate3d* to point to the correct coordinate
        // then we cast that pointer to an int32_t* (the type of x, y and z) and add the offset for the corresponding axis
        // finally we dereference
        current = *((int32_t*)(coords->coordinates + i) + offset);

        if(current != original_z[i])
            return 0;
    }

    return 1;
}

// https://en.wikipedia.org/wiki/Euclidean_algorithm
uint64_t greatest_common_denominator(uint64_t x, uint64_t y)
{
    uint64_t t;
    while(y != 0)
    {
        t = y;
        y = x % y;
        x = t;
    }
    return x;
}

uint64_t least_common_multiple(uint64_t x, uint64_t y)
{
    return x * y / greatest_common_denominator(x, y);
}

int main(int argc, char** argv)
{
    Coordinate3DVector *moons = cv3_create_coordinate_vector(4);
    Coordinate3DVector *velocities = cv3_create_coordinate_vector(4);
    coordinate3d empty = {0, 0, 0};
    coordinate3d coords[4] = {{-4, 3, 15},{-11, -10, 13},{2, 2, 18}, {7, -1, 0}};

    // Add moons
    for (uint32_t i = 0; i < 4; i++)
        cv3_add_coordinate(moons, coords[i]);
    
    // Add initial velocities
    for (uint32_t i = 0; i < 4; i++)
        cv3_add_coordinate(velocities, empty);

    // Store original moon positions and velocities to check overlap
    int32_t original_cx[4] = {moons->coordinates[0].x, moons->coordinates[1].x, moons->coordinates[2].x, moons->coordinates[3].x};
    int32_t original_cy[4] = {moons->coordinates[0].y, moons->coordinates[1].y, moons->coordinates[2].y, moons->coordinates[3].y};
    int32_t original_cz[4] = {moons->coordinates[0].z, moons->coordinates[1].z, moons->coordinates[2].z, moons->coordinates[3].z};

    int32_t original_vx[4] = {velocities->coordinates[0].x, velocities->coordinates[1].x, velocities->coordinates[2].x, velocities->coordinates[3].x};
    int32_t original_vy[4] = {velocities->coordinates[0].y, velocities->coordinates[1].y, velocities->coordinates[2].y, velocities->coordinates[3].y};
    int32_t original_vz[4] = {velocities->coordinates[0].z, velocities->coordinates[1].z, velocities->coordinates[2].z, velocities->coordinates[3].z};

    uint64_t found_x = 0, found_y = 0, found_z = 0;

    // starts with 1 to skip initial position
    uint64_t steps = 1;
    step(moons, velocities);
    while(steps == UINT64_MAX || !(found_x != 0 && found_y != 0 && found_z != 0))
    {
        if(check_state(moons, original_cx, 0) == 1 && check_state(velocities, original_vx, 0) == 1)
            found_x = steps;

        if(check_state(moons, original_cy, 1) == 1 && check_state(velocities, original_vy, 1) == 1)
            found_y = steps;

        if(check_state(moons, original_cz, 2) == 1 && check_state(velocities, original_vz, 2) == 1)
            found_z = steps;

        steps++;
        step(moons, velocities);
    }
    
    uint64_t first_denom = least_common_multiple(found_x, found_y);
    uint64_t result = least_common_multiple(first_denom, found_z);

    printf("It took %ld steps before returnin to initial state\n", result); // Output -> 292653556339368

    return 0;
}
