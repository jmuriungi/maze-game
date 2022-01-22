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

#include <sb7.h>
#include <shader.h>
#include <vmath.h>
#include <map> // For holding maze components

#include <loadingFunctions.h>  // header for object loading
#include <skybox.h>  // skybox header

// Implementations of class structures we use in our application
#include "camera.cpp"
#include "object.cpp"
#include "maze.cpp"
#include "ghost.cpp"

//Needed for file loading (also vector)
#include <string>
#include <fstream>
#include <vector>

#include <time.h> // random seed

// For error checking
#include <iostream>
#include <cassert>
#define GL_CHECK_ERRORS assert(glGetError()== GL_NO_ERROR);

class test_app : public sb7::application{
    public:

    void init(){
        // Set up appropriate title
        static const char title[] = "Midterm Project";
        sb7::application::init();
        memcpy(info.title, title, sizeof(title));

        //Make sure things are square to start with
        info.windowWidth = 1500; 
        info.windowHeight = 1500;
    }
    
    void startup(){
        //Initial movement speeds
        travelSpeed = 0.01f;
        camera_sensitivity = 0.01f;

        // Setting the maze to be a 50x50 randomized maze
        myMaze = simpleMaze(50,50,time(NULL));

        //////////////////////
        // Load Object Info //
        //////////////////////

        // Loading in the assets used for mazes        
        mazeComponents.insert(std::make_pair(WALL, object("./resources/textured_walls.obj", "./resources/pattern_wood.bmp")));
        mazeComponents.insert(std::make_pair(OPEN, object("./resources/textured_tile_1.obj", "./resources/pattern_wood.bmp")));
        mazeComponents.insert(std::make_pair(ITEM, object("./resources/textured_tile_1.obj", "./resources/pattern_wood.bmp")));
        mazeComponents.insert(std::make_pair(SOLUTION, object("./resources/tile_golden_gears.obj", "./resources/golden_gears.bmp")));
        mazeComponents.insert(std::make_pair(START, object("./resources/tile_golden_gears.obj", "./resources/golden_gears.bmp")));
        mazeComponents.insert(std::make_pair(END, object("./resources/tile_golden_gears.obj", "./resources/golden_gears.bmp")));
        GL_CHECK_ERRORS
        // Loading in 7 ghost objects                                                                                       VVVV Ensuring random seeding between ghosts (in case time(NULL) doesn't change between constructions)
        ghosts.push_back(ghost("./resources/pacManGhost.obj", "./resources/ghost_texture.bmp", myMaze.width, myMaze.length, time(NULL)+ghosts.size()));
        ghosts.push_back(ghost("./resources/pacManGhost.obj", "./resources/ghost_texture.bmp", myMaze.width, myMaze.length, time(NULL)+ghosts.size()));
        ghosts.push_back(ghost("./resources/pacManGhost.obj", "./resources/ghost_texture.bmp", myMaze.width, myMaze.length, time(NULL)+50000));
        ghosts.push_back(ghost("./resources/pacManGhost.obj", "./resources/ghost_texture.bmp", myMaze.width, myMaze.length, time(NULL)+128));
        ghosts.push_back(ghost("./resources/pacManGhost.obj", "./resources/ghost_texture.bmp", myMaze.width, myMaze.length, time(NULL)+ghosts.size()));
        ghosts.push_back(ghost("./resources/pacManGhost.obj", "./resources/ghost_texture.bmp", myMaze.width, myMaze.length, time(NULL)+54));
        ghosts.push_back(ghost("./resources/pacManGhost.obj", "./resources/ghost_texture.bmp", myMaze.width, myMaze.length, time(NULL)+47));
        GL_CHECK_ERRORS

        ////////////////////////////////
        //Set up Object Scene Shaders //
        ////////////////////////////////
        GLuint shaders[2];

        //Load scene rendering based shaders
        //These need to be co-located with main.cpp in src
        shaders[0] = sb7::shader::load(".\\src\\vs.glsl", GL_VERTEX_SHADER);
        compiler_error_check(shaders[0]);
        shaders[1] = sb7::shader::load(".\\src\\fs.glsl", GL_FRAGMENT_SHADER);
        compiler_error_check(shaders[1]);
        //Put together scene rendering program from the two loaded shaders
        rendering_program = sb7::program::link_from_shaders(shaders, 2, true);
        GL_CHECK_ERRORS

        /////////////////////////////////
        // Transfer Object Into OpenGL //
        /////////////////////////////////

        //Set up vao
        glUseProgram(rendering_program);
        glCreateVertexArrays(1,&vertex_array_object);
        glBindVertexArray(vertex_array_object);
        
        GL_CHECK_ERRORS
        ////////////////////////////////////
        // Grab IDs for rendering program //
        ////////////////////////////////////
        transform_ID = glGetUniformLocation(rendering_program,"transform");
        perspec_ID = glGetUniformLocation(rendering_program,"perspective");
        toCam_ID = glGetUniformLocation(rendering_program,"toCamera");
        vertex_ID = glGetAttribLocation(rendering_program,"obj_vertex");
        uv_ID = glGetAttribLocation(rendering_program,"obj_uv");        

        ///////////////////////////
        //Set up Skycube shaders //
        ///////////////////////////
        // Placeholders for loaded shaders
        GLuint sc_shaders[2];

        //Load Skycube based shaders
        //These need to be co-located with main.cpp in src
        sc_shaders[0] = sb7::shader::load(".\\src\\sc_vs.glsl", GL_VERTEX_SHADER);
        compiler_error_check(sc_shaders[0]);
        sc_shaders[1] = sb7::shader::load(".\\src\\sc_fs.glsl", GL_FRAGMENT_SHADER);
        compiler_error_check(sc_shaders[1]);

        //Put together Sky cube program from the two loaded shaders
        sc_program = sb7::program::link_from_shaders(sc_shaders, 2, true);
        GL_CHECK_ERRORS

        /////////////////////
        //Load Skycube info//
        /////////////////////
        //skycube_vertices holds triangle form of a cube
        createCube(skycube_vertices);
        //Set up Vertex Array Object and associated Vertex Buffer Object
        GLuint vBufferObject; //We only need this to associate with sc_vertex_array_object
        glGenBuffers(1,&vBufferObject); //Create the buffer id
        glBindBuffer( GL_ARRAY_BUFFER, vBufferObject);
        glBufferData( GL_ARRAY_BUFFER,
                skycube_vertices.size() * sizeof(skycube_vertices[0]), //Size of element * number of elements
                skycube_vertices.data(),                               //Actual data
                GL_STATIC_DRAW);                                       //Set to static draw (read only)  
        
        glGenVertexArrays(1, &sc_vertex_array_object); // Get ID for skycube vao
        glBindVertexArray(sc_vertex_array_object);
        glEnableVertexAttribArray(0); //Enable Vertex Attribute Array
        glBindBuffer( GL_ARRAY_BUFFER, vBufferObject);
        glVertexAttribPointer( 0, 4, GL_FLOAT, GL_FALSE, 0, NULL); //Linking the buffer filled above to a vertex attribute
        GL_CHECK_ERRORS

        //Set up texture information
        glActiveTexture(GL_TEXTURE0);     //Set following data to GL_TEXTURE0
        glGenTextures(1,&sc_map_texture); //Grab texture ID
        //Call a file loading function to load in textures for skybox
        loadCubeTextures(".\\resources\\Skycube\\",sc_map_texture);
        GL_CHECK_ERRORS

        //Get uniform handles for perspective and camera matrices
        sc_Perspective = glGetUniformLocation(sc_program,"perspective");
        sc_Camera= glGetUniformLocation(sc_program,"toCamera");
        GL_CHECK_ERRORS

        /////////////////////
        // Camera Creation //
        /////////////////////
        //Setting initial camera values
        mainCamera.setFOV(67.0f); // 67 degrees initially
        mainCamera.setPos(vmath::vec3(myMaze.width/2,0.0f,(myMaze.length-2))); // Starting the camera at the start of the maze
        mainCamera.setRatio(info.windowWidth / info.windowHeight); // Setting ratio to same as window

        //Link locations to Uniforms
        glUseProgram(sc_program);
        glUniformMatrix4fv(sc_Perspective,1,GL_FALSE, mainCamera.getTranslationProjection());
        glUniformMatrix4fv(sc_Camera,1,GL_FALSE, mainCamera.getProjection());
        GL_CHECK_ERRORS

        // General openGL settings
        //src:: https://github.com/capnramses/antons_opengl_tutorials_book/tree/master/21_cube_mapping
        glEnable( GL_DEPTH_TEST );          // enable depth-testing
        glDepthFunc( GL_LESS );             // depth-testing interprets a smaller value as "closer"
        glEnable( GL_CULL_FACE );           // cull face
        glCullFace( GL_BACK );              // cull back face
        glFrontFace( GL_CCW );              // set counter-clock-wise vertex order to mean the front
        glClearColor( 0.2, 0.2, 0.2, 1.0 ); // grey background to help spot mistakes

        //End of set up check
        GL_CHECK_ERRORS
    }

    void shutdown(){
        //Clean up Buffers
        glDeleteVertexArrays(1, &sc_vertex_array_object);
        glDeleteTextures(1,&sc_map_texture);
        glDeleteProgram(sc_program);
    }

    // Method to check if the camera is inside any object (collision)
    // Input: mainCamera's position, 
    // Output: returns true if camera is inside any obejct
    bool checkCollisions() {
        // Idea for "collisions" against walls without needing object data
        return myMaze.checkWalls(mainCamera.getPos()[0],mainCamera.getPos()[2], 0.2f);
    }

    // Our method for handling input (seperate from onKey for smoother movement)
    // Calls checkCollisions() to undo movement if camera/player is in/too close to object
    // Input: none
    // Output: movement on keypress
    void handleInput() {
        //float travelSpeed = 0.05f;
        //forward movement
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            mainCamera.moveFoward(travelSpeed);
            // If this movement causes a collision, we undo that movement
            if (checkCollisions()) {
                mainCamera.moveFoward(-travelSpeed);
            }
        }
        //move left
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            // We would need to check for collision here
            mainCamera.moveLeft(travelSpeed);
            // If this movement causes a collision, we undo that movement
            if (checkCollisions()) {
                mainCamera.moveLeft(-travelSpeed);
            }
        }
        // move right
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            // We would need to check for collision here
            mainCamera.moveLeft(-travelSpeed);
            // If this movement causes a collision, we undo that movement
            if (checkCollisions()) {
                mainCamera.moveLeft(travelSpeed);
            }
        }
        // move backwards
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            // We would need to check for collision here
            mainCamera.moveFoward(-travelSpeed);
            // If this movement causes a collision, we undo that movement
            if (checkCollisions()) {
                mainCamera.moveFoward(travelSpeed);
            }
        }
        /* Ability to go up or down (doesn't change collision) */
        // move up
        if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
            mainCamera.moveUp(travelSpeed);
        }//move down
        if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
            mainCamera.moveUp(-travelSpeed);
        }
        // Camera Rotation using arrow keys
        //look right
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
            mainCamera.turnRight(camera_sensitivity);
        }//look left
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
            mainCamera.turnRight(-camera_sensitivity);
        }//look up
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
            mainCamera.turnUp(camera_sensitivity);
        }//look down
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
            mainCamera.turnUp(-camera_sensitivity);
        }
    }

    // Core render behavior
    void render(double curTime){
        glViewport( 0, 0, info.windowWidth, info.windowHeight ); //Set Viewport information
        //Clear output
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        runtime_error_check(1);

        //Draw skyCube
        drawSkyCube(curTime);

        runtime_error_check(2);

        /////////////////////////////////////////////////////////////////////////////////
        // This would be where you want to call another program to render your 'stuff' //
        // This could also go into a function for organizational ease                  //
        /////////////////////////////////////////////////////////////////////////////////
        handleInput(); // Check for movement input
        
        // render loop, go through each object and render it!
        glUseProgram(rendering_program); //activate the render program
        glBindVertexArray(vertex_array_object); //Select base vao
        
        // Setting transforms from the camera that would be applied to all objects
        glUniformMatrix4fv(perspec_ID, 1,GL_FALSE, mainCamera.getPerspective()); //Load camera projection
        glUniformMatrix4fv(toCam_ID, 1,GL_FALSE, mainCamera.getTranslationProjection()); //Load in view matrix for camera
        
        // Rendering the maze by checking each cell and rendering the appriopriate object
        for (int col = 0; col < myMaze.width; col++) {
            for (int row = 0; row < myMaze.length; row++) {
                glUniformMatrix4fv(transform_ID, 1, GL_FALSE, vmath::translate(vmath::vec3(float(col), 0.0, float(row)))); //Load in transform for where this object is in the maze grid
                mazeComponents.find(myMaze.maze[col][row])->second.renderObject(vertex_ID, uv_ID); //once location in maze is identified, render maze component there!
            }
            runtime_error_check(3);
        }
        // Have each ghost perform their move and collision check with the camera/player, then render them
        for (int i = 0; i < ghosts.size(); i++) {
            ghosts[i].move(curTime); // move ghost in maze according to their predetermined path
            // If the ghost's movement led to a collision with the camera/player, then reset the player's position
            if (ghosts[i].isSpooked(mainCamera.getPos())) {
                mainCamera.setPos(vmath::vec3(myMaze.width/2,0.0f,(myMaze.length-2))); // Moving the camera/player back to the start of the maze
            }
            glUniformMatrix4fv(transform_ID, 1, GL_FALSE, (ghosts[i].obj2world)); //Load in transform for this object
            ghosts[i].renderObject(vertex_ID, uv_ID);
        }

        // for each ghost in ghost vector -> render the ghost
        runtime_error_check(4);
    }
    // Draw Skycube function
    // Input: current time
    // Output: skycube/bitmap images forming our surroundings
    void drawSkyCube(double curTime){
        glDepthMask( GL_FALSE ); //Used to force skybox 'into' the back, making sure everything is rendered over it
        glUseProgram( sc_program ); //Select the skycube program
        glUniformMatrix4fv( sc_Perspective, 1, GL_FALSE, mainCamera.getPerspective()); //Update the projection matrix (if needed)
        glUniformMatrix4fv( sc_Camera, 1, GL_FALSE, mainCamera.getProjection()); //Update the projection matrix (if needed)
        glActiveTexture( GL_TEXTURE0 ); //Make sure we are using the CUBE_MAP texture we already set up
        glBindTexture( GL_TEXTURE_CUBE_MAP, sc_map_texture ); //Link to the texture
        glBindVertexArray( sc_vertex_array_object ); // Set up the vertex array
        glDrawArrays( GL_TRIANGLES, 0, skycube_vertices.size() ); //Start drawing triangles
        glDepthMask( GL_TRUE ); //Turn depth masking back on
        runtime_error_check();
    }
    // Resets camera when window is resized
    // Input: window dimentions
    // Output: new camera position
    void onResize(int w, int h) {
        info.windowWidth = w;
        info.windowHeight = h;
        // Recalculate the projection matrix used by camera
        mainCamera.setRatio(w / h); 
    }

    // onKey function for handling camera movement/turns (more discrete button presses) and other button functionality
    // Input: key pressed, action
    // Output: action resultant from keypress
    void onKey(int key, int action) {
        if (action) {        // If something did happen
            switch (key) { // Select an action
                case(GLFW_KEY_F1):// F1 toggles wireframe mode for all objects in scene
                    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                    break;
                case(GLFW_KEY_F2):// F2 toggles back to material view for all objects in scene
                    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                    break;
                //control movement speed with '+', '-' buttons
                case(GLFW_KEY_MINUS):
                    travelSpeed -= 0.005;
                    if(travelSpeed < 0)
                    {
                        travelSpeed = 0.0001;
                    }
                    break;
                case(GLFW_KEY_EQUAL):
                    travelSpeed += 0.005;
                    break;
                //control speed camera changes with '[', ']' buttons
                case(GLFW_KEY_LEFT_BRACKET):
                    camera_sensitivity -= 0.005;
                    if(camera_sensitivity < 0)
                    {
                        camera_sensitivity = 0.0001;
                    }
                    break;
                case(GLFW_KEY_RIGHT_BRACKET):
                    camera_sensitivity += 0.005;
                    break;    
            }
        }
    }

    void runtime_error_check(GLuint tracker = 0) {
        GLenum err = glGetError();
        
        if (err) {
            char buf[50];
            sprintf(buf, "Error(%d) = %x", tracker, err);
            MessageBoxA(NULL, buf, "Error running program!", MB_OK);
        }
    }

    void compiler_error_check(GLuint shader) {
        GLint isCompiled = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
        if (isCompiled == GL_FALSE) {
            GLint maxLength = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

            // The maxLength includes the NULL character
            std::vector<GLchar> errorLog(maxLength);
            glGetShaderInfoLog(shader, maxLength, &maxLength, &errorLog[0]);

            if (maxLength == 0) {
                char buf[50];
                sprintf(buf, "Possibly shaders not found!", maxLength);
                MessageBoxA(NULL, buf, "Error in shader compilation", MB_OK);
            } else {
                // Provide the infolog in whatever manor you deem best.
                MessageBoxA(NULL, &errorLog[0], "Error in shader compilation", MB_OK);
            }

            // Exit with failure.
            glDeleteShader(shader); // Don't leak the shader.
        }
    }

    private:
        //Scene Rendering Information
        GLuint rendering_program; //Program reference for scene generation
        GLuint vertex_array_object;

        float travelSpeed, camera_sensitivity;
        
        //Uniform attributes for Scene Render
        GLuint transform_ID; //Dynamic transform of object
        GLuint perspec_ID;   //Perspective transform
        GLuint toCam_ID;     //World to Camera transform
        GLuint vertex_ID;    //This will be mapped to different objects as we load them
        GLuint uv_ID;


        std::map<FILL_TYPE, object> mazeComponents;// tiles, cubes
        std::vector<ghost> ghosts; // ghost objects that the player must avoid

        //Data for Skycube
        GLuint sc_program; //Program reference

        GLuint sc_vertex_array_object;
        GLuint sc_map_texture;

        //TODO:: Rename these better names
        GLuint sc_Camera;
        GLuint sc_Perspective;

        std::vector<vmath::vec4> skycube_vertices; // List of skycube vertexes

        // Camera properties and behaviors encapsulated in custome camera class
        camera mainCamera;
        // Maze object data and methods
        simpleMaze myMaze;
};

DECLARE_MAIN(test_app);
