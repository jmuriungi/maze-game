#ifndef _GHOST_H_
#define _GHOST_H_

#include "../headers/object.h"
#include <time.h>

// Ghost to add behaviors and values along with inheriting from object (Ghost IS-A object)
class ghost : public object {
protected:
    // Along with normal object values, additional values for the parametric function
    float x_radius, z_radius, time_offset, speed;
    int centerX, centerZ;
public:
    // Constructor
    // Input for file of object and texture files, along with dimensions of maze and a seed for random number generation
    ghost(std::string filePath, std::string textPath, int mazeW, int mazeL, int seed);

    // Parametric movement function, move in oval shape with up-down bobbing
    void move(float time);

    // Mehtod for checking if a point is to be "spooked" (collision)
    bool isSpooked(vmath::vec3 point);

};

#endif