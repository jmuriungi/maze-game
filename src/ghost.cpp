#include "../headers/ghost.h"

// Ghost object constructor
// Takes in     obj file path,      bmp texture path,   maze width, maze length and a random seed.
ghost::ghost(std::string filePath, std::string textPath, int mazeW, int mazeL, int seed) {
    srand(seed);
    this->speed = float(rand() % 20) / 20.0f;
    // Random position with in the maze
    this->position = vmath::vec3(0.0f,//(float)(rand() % (mazeW - mazeW/2)), 
                                 0.0f,
                                 0.0f);

    this->centerX = rand() % mazeW;
    this->centerZ = rand() % mazeL;

    this->x_radius = rand() % mazeW/2; // limit range of radius to width of maze
    this->z_radius = rand() % mazeL/2; // limit range of radius to length of maze
    this->time_offset = (rand() % 1000 - 500)/2.0f; // Time offset changes how far along their elliptic path they are going already

    this->bank = 0;
    this->heading = 0;
    this->pitch = 0;
    // Load from file for other data
    load_from_file(filePath);

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

// Move function for ghost object
// Takes in time; objects will be moving as long as program is running
// Output: ghosts roam around maze
void ghost::move(float time) {
    float x,y,z, dx,dz; // position and derivative variables

    x = (x_radius * sin(time*speed + time_offset)) + this->centerX;  //ghost direction in the x
    y = 0.25f*sin(2.0*time*speed + time_offset);        //ghost direction in the y
    z = (z_radius * cos(time*speed + time_offset)) + this->centerZ;  //ghost direction in the z
    
    this->setPos(vmath::vec3(x,y,z));   // update ghost's position

    // Use derivative to get some sense of direction for heading
    dx = x_radius * cos(time*speed + time_offset) * speed;
    dz = -z_radius * sin(time*speed + time_offset) * speed;

    if (dz != 0.0f) {
        this->setHeading(atan(dx / dz) - M_PI/2.0f);
    }
    else {
        // if dz = 0, then set it to a really small value to preseve heading orientation
        dz = 0.000001f;
        this->setHeading(atan(dx / dz) - M_PI/2.0f);
    }
    // Correcting heading orientations due to domain range of arctan()
    if (dz < 0) {
        this->setHeading(this->heading + M_PI);
    }

}
// Checks to see if camera/player is inside ghost object
// Input: camera position
// Output: True or false
bool ghost::isSpooked(vmath::vec3 point)
{
    // Using object method for isInside with max distance of 0.1
    return this->isInside(point, 0.1f);
}
