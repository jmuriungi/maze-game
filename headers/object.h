#ifndef _OBJFILE_H_
#define _OBJFILE_H_

#include <string>
#include <vector>
#include "../headers/sb7-h/sb7.h"
#include "../headers/sb7-h/vmath.h"

// Structure to hold properties of a rendered object
// Contents based on Scott's example of object loading
struct object {
protected:
    // Poistion of the object, unused by maze cells
    vmath::vec3 position;
    // Orientation of the object in radians
    float bank, heading, pitch;

    bool textured;
public:
    object();
    // Constuctor that takes in string to where the .obj file is located
    // Defaults the orientation to 0,0,0
    object(std::string filePath, std::string textPath = "");

    // Points that make up each triangle
    std::vector<vmath::vec4> verticies;
    std::vector<vmath::vec4> normals;
    std::vector<vmath::vec2> uvs;
    // Points that are contained in each face to help define planes
    std::vector<vmath::vec4> facePoints;
    // Normal vectors for each face
    std::vector<vmath::vec4> faceNormals;
    // Matrix for transitioning the object to world space
    vmath::mat4 obj2world;
    vmath::mat4 orientation;
    // ID for the vertex buffer that this object resides in
    GLuint vertices_buffer_ID;
    // ID for the UV buffer
    GLuint uv_buffer_ID;
    // ID for the texture info tied to this object
    GLuint texture_ID;

    void updateMatrices(); // Called whenever position or orientation is changed
    // Simple setters that call updateMatrices()
    void setPos(vmath::vec3 new_pos);
    void setHeading(float new_heading);
    void setBank(float new_bank);
    void setPitch(float new_pitch);

    // Getter for position
    vmath::vec3 getPos();
    // Getter for if this object is textured
    bool isTextured();

    // Method used in constructor to properly load contents of .obj file
    void load_from_file(std::string filePath);

    // Method used in constructor for loading texture with given file path to it
    void load_texture(std::string textPath);

    // Method used in constructor to derive contents for facePoints and faceNormals
    void deriveFacePoints();
    // Method for getting the distance from a point to a given face
    // Input: c_pos - 3D point to check
    //        face_id - the given face to check
    // Output: distance from the face
    float getDistanceFromFace(vmath::vec3 c_pos, int face_id);

    // Method for determining if a point is inside the object
    // Input: given point to check, and float value for each distance to check against
    // Output: returns true if point is within the object (by being less than max_distance for all faces)
    bool isInside(vmath::vec3 point, float max_distance);

    // For encapsulating the general process of rendering an object
    void renderObject(int vertexID, int uvID);

};

#endif