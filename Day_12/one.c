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

uint32_t get_energy(Coordinate3DVector* moons, Coordinate3DVector* velocities)
{
    coordinate3d auxMoon, auxVel;
    uint32_t to_return = 0;
    for(uint32_t i = 0; i < 4; i++) 
    {
        // absolute values
        auxMoon.x = (moons->coordinates[i].x < 0) ? moons->coordinates[i].x * (-1) : moons->coordinates[i].x;
        auxMoon.y = (moons->coordinates[i].y < 0) ? moons->coordinates[i].y * (-1) : moons->coordinates[i].y;
        auxMoon.z = (moons->coordinates[i].z < 0) ? moons->coordinates[i].z * (-1) : moons->coordinates[i].z;

        auxVel.x = (velocities->coordinates[i].x < 0) ? velocities->coordinates[i].x * (-1) : velocities->coordinates[i].x;
        auxVel.y = (velocities->coordinates[i].y < 0) ? velocities->coordinates[i].y * (-1) : velocities->coordinates[i].y;
        auxVel.z = (velocities->coordinates[i].z < 0) ? velocities->coordinates[i].z * (-1) : velocities->coordinates[i].z;

        to_return = to_return + (auxMoon.x + auxMoon.y + auxMoon.z) * (auxVel.x + auxVel.y + auxVel.z);
    }

    return to_return;
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

    uint32_t steps = 1000;
    for (uint32_t i = 0; i < steps; i++)
        step(moons, velocities);

    printf("Energy after %d steps is %d\n", steps, get_energy(moons, velocities)); // Output -> 722

    return 0;
}
