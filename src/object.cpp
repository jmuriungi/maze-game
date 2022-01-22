/*
 * Andrew O'Kins, Jeremy Muriungi
 * Scott Griffith
 * CS 357 Computer Graphics
 * PROJECT 4 - Midterm Project: Space Maze
 * 04/18/2021
 * 
 * Resources used:
 * Based on work by Graham Sellers and OpenGL SuperBible7
 * 3D Math Primer
 * In-Class Examples
 * 
 */

#include "../headers/object.h"
#include <transformDerive.h>
#include <loadingFunctions.h>
#include <fstream>
#include <iostream>

#include <sb7.h> // For access to openGL methods

object::object() {
    this->position = vmath::vec3();
    this->bank = 0;
    this->heading = 0;
    this->pitch = 0;
   
}

// Constructor
object::object(std::string objPath, std::string textPath) {
    this->position = vmath::vec3();
    this->bank = 0;
    this->heading = 0;
    this->pitch = 0;
    // Load from file for other data
    load_from_file(objPath);

    //For each object in objects, set up openGL buffers
    glGenBuffers(1,&this->vertices_buffer_ID); //Create the buffer id for this object
    glBindBuffer( GL_ARRAY_BUFFER, this->vertices_buffer_ID);
    glBufferData( GL_ARRAY_BUFFER,
        this->verticies.size() * sizeof(this->verticies[0]), //Size of element * number of elements
        this->verticies.data(),                                   //Actual data
        GL_STATIC_DRAW);                                               //Set to static draw (read only)  


    // Now that we have the data of the object, we can derive the average points of each face
    deriveFacePoints();
    updateMatrices();
    // Load object textures
    if (textPath != "") {
        load_texture(textPath);
        glGenBuffers(1, &this->uv_buffer_ID);
        glBindBuffer(GL_ARRAY_BUFFER, this->uv_buffer_ID);
        glBufferData(GL_ARRAY_BUFFER, this->uvs.size() * sizeof(this->uvs[0]),
                    this->uvs.data(),
                    GL_STATIC_DRAW
        );
        this->textured = true;
    }
    else {
        this->textured = false;
    }
}

// Method used in constructor to properly load contents of .obj file (implementation largely from Scott's example)
// Input is string to where the file is
// Output is verticies, uvs, and normals vectors in struct are set according to file contents
void object::load_from_file(std::string filePath) {
    //File to load in
    std::ifstream in(filePath, std::ios::in);

    //Check to make sure file opened
    if (!in) {
        char buf[50];
        sprintf(buf, "OBJ file not found!");
        exit(EXIT_FAILURE);
    }

    //Temp vectors to hold data
    //These will need to be indexed into the output vectors based on face info
    std::vector<vmath::vec4> tempVert; // from verticies lines 'v <x> <y> <z>'
    //This might need to be a more complicated structure, we can get away with strict indexing
    std::vector<GLuint> tempFace; // from face line 'f <v1>/<t1>/<n1> <v2>/<t2>/<n2> <v3>/<t3>/<n3>', should be indexes
    std::vector<vmath::vec2> tempUVs; // from texture line 'vt <x> <y>'
    std::vector<vmath::vec4> tempNorm; // from a normal line 'vn <x> <y> <z>' //Should this be a vec3 or vec4?

    std::string line;       // Complete line pulled from file    
    std::string sub = "";   // Substring working space
    GLfloat num;            // Temp floats from line processing
    
    while (std::getline(in, line)) { //grab every line of the file
        //Depending on what is in the line, parse differently 
        
        if (line.substr(0,2) == "v ") {
            //Process vertex line
            sub = line.substr(2); //Isolate substring without 'v'

            //vector to be filled
            vmath::vec4 tVec(0.0f,0.0f,0.0f,1.0f);
            
            //Expect 3 numbers, iterate three times
            //Isolate number and clip substring [x,y,z,1]
            tVec[0] = std::stof(pop_substr(sub," "));
            tVec[1] = std::stof(pop_substr(sub," "));
            tVec[2] = std::stof(pop_substr(sub," "));
            
            //Push vec4 onto verticies vector
            tempVert.push_back(tVec);
           
        } else if (line.substr(0,2) == "f ") {
            //Process face line 
            // Faces line f 14/25/9 60/19/9 56/97/9 : f <vertex1>/<texture1>/<normal1> <vertex2>/<texture2>/<normal2> <vertex3>/<texture3>/<normal3>
            sub = line.substr(2); //Current sub string of line
            //Expect 3 number sets, loop three times
            for(int i = 0; i < 3; i++) {
                //Isolate next triangle set
                //First before / /
                tempFace.push_back( std::stoi(pop_substr(sub,"/")) ); //sub_part 0 == vertex index
                //second value in triplet (between the / and /)
                tempFace.push_back( std::stoi(pop_substr(sub,"/")) ); //sub_part 1 == texture coordinate index
                //third value in triplet (after both /, " " up next)
                tempFace.push_back( std::stoi(pop_substr(sub," ")) ); //sub_part 2 == normal index                    
            }
            //Already pushed info onto faces
        
        } else if (line.substr(0,3) == "vt ") {
            //Process Texture mappings
            sub = line.substr(3); //Isolate substring without 'vt '

            vmath::vec2 tUV(0.0f,0.0f);
            //Expect 2 numbers, loop twice
            tUV[0] = std::stof(pop_substr(sub," ")); //x
            tUV[1] = std::stof(pop_substr(sub," ")); //y

            //Push vec2 onto texture vector
            tempUVs.push_back(tUV);                

        } else if (line.substr(0,3) == "vn ") {
            //Process normal line
            sub = line.substr(3); //Isolate substring without 'vt '

            //4D? is w:0?
            vmath::vec4 tNorm(0.0f,0.0f,0.0f,0.0f);

            //Expect 3 numbers, loop thrice
            //Isolate number and clip substring
            tNorm[0] = std::stof(pop_substr(sub," "));
            tNorm[1] = std::stof(pop_substr(sub," "));
            tNorm[2] = std::stof(pop_substr(sub," "));
            //Push normal onto tempNormal vector
            tempNorm.push_back(tNorm);

        } else {
            //other kind of line, ignoring
        }
    } //end of line parsing

    //Clear out output vectors (just to be safe)
    this->verticies.clear();
    this->uvs.clear();
    this->normals.clear();

    //At this point out temp vectors are full of data
    // tempVert, tempUVs and tempNorm are indexed (starting at 0) in file order
    // tempFace correlates everything together in sets of 9 values (three triplets)
    // Using the data in tempFace the output vectors will be filled with the correct (in order) data
    //                   0    1    2    3    4    5    6    7    8
    // Faces striping: <v1>/<t1>/<n1> <v2>/<t2>/<n2> <v3>/<t3>/<n3>
    //Because the data in tempFace is striped buy sets of three triplets, step forward by 9 each time
    for(int i = 0; i < tempFace.size(); i += 9 ){
        //Pull data into verticies
        //                                   VVV Index offset pattern
        //                          VVV Holds vertex index to pull from tempVery (offset from starting at 1 to 0)    
        //                 VVV Indexed vertex info
        this->verticies.push_back(tempVert[tempFace[i+0]-1]); //v1
        this->verticies.push_back(tempVert[tempFace[i+3]-1]); //v2
        this->verticies.push_back(tempVert[tempFace[i+6]-1]); //v3

        //Pull data into uvs
        this->uvs.push_back(tempUVs[tempFace[i+1]-1]); //uv1
        this->uvs.push_back(tempUVs[tempFace[i+4]-1]); //uv2
        this->uvs.push_back(tempUVs[tempFace[i+7]-1]); //uv3

        //Pull data into normals
        this->normals.push_back(tempNorm[tempFace[i+2]-1]); //n1
        this->normals.push_back(tempNorm[tempFace[i+5]-1]); //n1
        this->normals.push_back(tempNorm[tempFace[i+8]-1]); //n1
    }
}
// Load textures function
// Input: file path for texture file
// Output: assigns texture to object
void object::load_texture(std::string textPath) {
    unsigned char * imgData;
    unsigned int imgWidth;
    unsigned int imgHeight;

    load_BMP(textPath, imgData, imgWidth, imgHeight);
        
    glGenTextures(1, &this->texture_ID);
    glBindTexture(GL_TEXTURE_2D, this->texture_ID);
    glTexImage2D(GL_TEXTURE_2D, // type of texture (2d image)
                0, GL_RGBA, // simple detail with RGB and A values
                imgWidth, imgHeight, // Given dimensions of the image
                0, GL_RGBA, GL_UNSIGNED_BYTE, // no border, input is RGBA that is comprised of unsigned bytes
                imgData); // pointer to the data itself
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    delete [] imgData;
}

// Takes every three vertices in vertices array and stores the average of them inside average vector
// Input: none
// Output: normals for every face of object
void object::deriveFacePoints() {
    // Clear if there was anything in there already
    this->facePoints.clear();
    this->faceNormals.clear();
    // Allocate space needeed
    this->facePoints = std::vector<vmath::vec4>(verticies.size()/3, vmath::vec4(0.0f,0.0f,0.0f,0.0f));
    this->faceNormals = std::vector<vmath::vec4>(normals.size()/3, vmath::vec4(0.0f,0.0f,0.0f,0.0f));
    // Store the average for each face vertex point (group of three vertices) to get facePoint
    for (int i = 0; i < verticies.size() / 3; i++) {
        facePoints[i] = verticies[3*i]   / 3.0f;
        facePoints[i] += verticies[3*i+1] / 3.0f;
        facePoints[i] += verticies[3*i+2] / 3.0f;
        facePoints[i][3] = 0.0f;
        // The normal of a given face is the normalized sum of each vertex's normal
        faceNormals[i] = vmath::normalize(normals[3*i] + normals[3*i+1] + normals[3*i+2]);
    }
}
// Calculate distance from camera/player to object face
// Input: camera position, object face id
// Output: distance
float object::getDistanceFromFace(vmath::vec3 c_pos, int face_id)
{
    // If invalid index, returnning large distance as temporary handler
    if (face_id > facePoints.size()-1 || face_id < 0) {
        return 999.9f;
    }
    else {
        float distance;
        // Calculate the distance
        // Get the world space coordinate for the face
        vmath::vec4 faceWorld = (facePoints[face_id] * this->orientation) + vmath::vec4(this->position, 0.0f);
        // Get the normal with orientation considered
        vmath::vec4 normalWorld = faceNormals[face_id] * this->orientation;
        // Use the dot product to get the distance
        distance = vmath::dot((vmath::vec4(c_pos, 1.0f) - (faceWorld)), (normalWorld));
        return distance;
    }
}

// Method that should called whenever changing position or orientation values for the object 
// Input: none
// Output: new object position and/or orientation
void object::updateMatrices() {
    // Derive orientation
    this->orientation = getRotationMatrix(this->bank, this->heading, this->pitch); //
    // Derive obj2world using orientation
    this->obj2world = vmath::translate(this->position) * this->orientation;
}

// Setter for position
// Input: new_pos - position to set the object to
// Output: position updated as well as matrices
void object::setPos(vmath::vec3 new_pos) {
    this->position = new_pos;
    // Update the matrices with new position
    updateMatrices();
}

// Setter for heading
// Input: new_heading - heading to set the object to
// Output: heading updated as well as matrices
void object::setHeading(float new_heading)
{
    this->heading = new_heading;
    //update the matrices with new heading
    updateMatrices();
}

// Setter for bank
// Input: new_bank - bank to set the object to
// Output: bank updated as well as matrices
void object::setBank(float new_bank)
{
    this->bank = new_bank;
    //update the matrices with new bank
    updateMatrices();
}

// Setter for pitch
// Input: new_pitch - pitch to set the object to
// Output: head updated as well as matrices
void object::setPitch(float new_pitch)
{
    this->pitch = new_pitch;
    //update the matrices with new heading
    updateMatrices();
}

// Getter for textured boolean
// Input: none
// Ouput: True/False
bool object::isTextured() {
    return this->textured;
}

// Redner object function
// Input: object's vertices and texture uv's
// Output: renders textured object
void object::renderObject(int vertexID, int uvID) {
    // Linking vertex buffer
    glEnableVertexAttribArray(vertexID); //Recall the vertex ID
    glBindBuffer(GL_ARRAY_BUFFER, this->vertices_buffer_ID);//Link object buffer to vertex_ID
    glVertexAttribPointer( // Index into the buffer
            vertexID,  // Attribute in question
            4,         // Number of elements per vertex call (vec4)
            GL_FLOAT,  // Type of element
            GL_FALSE,  // Normalize? Nope
            0,         // No stride (steps between indexes)
            0);        // initial offset

    // Linking UV buffer for textures (if textured)
    // If is textured we will need to bind that
    if (this->isTextured()) {
        glBindTexture(GL_TEXTURE_2D, this->texture_ID);

        glEnableVertexAttribArray(uvID); //Recall the vertex ID
        glBindBuffer(GL_ARRAY_BUFFER, this->uv_buffer_ID);//Link object buffer to vertex_ID
        glVertexAttribPointer( //Index into the buffer
                uvID, //Attribute in question
                2,         //Number of elements per vertex call (vec2)
                GL_FLOAT,  //Type of element
                GL_FALSE,  //Normalize? Nope
                0,         //No stride (steps between indexes)
                0);       //initial offset
    }
    glDrawArrays( GL_TRIANGLES, 0, this->verticies.size());
}

// Checks if player/camera is inside object, used for collision btwn camera and object
// Input: camera position, max distance
// Output: inside (if camera is inside object)
bool object::isInside(vmath::vec3 point, float max_distance) {
    bool inside = true;
    float distance;
    // Check every face for this object
    for (int face_i = 0; face_i < this->facePoints.size(); face_i++) {
        // Get the distance for a given face
        distance = this->getDistanceFromFace(point, face_i);
        // If the distance is outside the bound for any face, then we are for sure not inside the object
        if (distance > max_distance) {
            inside = false;
        }
    }
    // Return resulting conclusion if we are inside or not
    return inside;
}
