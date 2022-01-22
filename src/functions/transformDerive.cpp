#ifndef _TRANSFORMS_CPP_
#define _TRANSFORMS_CPP_
// Methods for generating rotation matrices
#include <transformDerive.h>

// Generate rotation matrix using euler angles
// Input: x, y, z - euler angles for rotation
// Output: A 4x4 matrix of rotation is returned
vmath::mat4 getRotationMatrix(float x, float y, float z) {
    return getRotationY(y) * getRotationZ(z) * getRotationX(x);
}

// Generate rotation matrix that is for rotation around X axis given angle
vmath::mat4 getRotationX(float angle) {
    return vmath::mat4 ( // Rotation about the x axis
        vmath::vec4(1.0f, 0.0f, 0.0f, 0.0f),
        vmath::vec4(0.0f,  cos(angle), sin(angle), 0.0f),
        vmath::vec4(0.0f, -sin(angle), cos(angle), 0.0f),
        vmath::vec4(0.0f,  0.0f, 0.0f, 1.0f));
}

// Generate rotation matrix that is for rotation around Y axis given angle
vmath::mat4 getRotationY(float angle) {
    return vmath::mat4( // Rotation about the y axis
        vmath::vec4(cos(angle), 0.0f,-sin(angle), 0.0f),
        vmath::vec4(0.0f,       1.0f, 0.0f,       0.0f),
        vmath::vec4(sin(angle), 0.0f, cos(angle), 0.0f),
        vmath::vec4(0.0f,       0.0f, 0.0f,       1.0f));
}

// Generate rotation matrix that is for rotation around Z axis given angle
vmath::mat4 getRotationZ(float angle) {
    return vmath::mat4( // Rotation about the z axis
        vmath::vec4( cos(angle), sin(angle), 0.0f, 0.0f),
        vmath::vec4(-sin(angle), cos(angle), 0.0f, 0.0f),
        vmath::vec4(       0.0f,       0.0f, 1.0f, 0.0f),
        vmath::vec4(       0.0f,       0.0f, 0.0f, 1.0f));
}

// Generate rotation matrix that is for rotation around a given axis
vmath::mat3 getRotataionN(float angle, vmath::vec3 axis) {
    return vmath::mat3(
        // Row 1
        vmath::vec3(axis[0]*axis[0]*(1 - cos(angle)) + cos(angle),
        axis[0]*axis[1]*(1 - cos(angle)) + axis[2]*sin(angle),
        axis[0]*axis[2]*(1 - cos(angle)) - axis[1]*sin(angle)),
        // Row 2
        vmath::vec3(axis[0]*axis[1]*(1 - cos(angle)) - axis[2]*sin(angle),
        axis[1]*axis[1]*(1 - cos(angle)) + cos(angle),
        axis[1]*axis[2]*(1 - cos(angle)) - axis[0]*sin(angle)),
        // Row 3
        vmath::vec3(axis[0]*axis[2]*(1 - cos(angle)) + axis[1]*sin(angle),
        axis[1]*axis[2]*(1 - cos(angle)) + axis[0]*sin(angle),
        axis[2]*axis[2]*(1 - cos(angle)) + cos(angle))
    );
}

#endif
