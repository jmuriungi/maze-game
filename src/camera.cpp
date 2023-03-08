#include "../headers/camera.h"
#include "./functions/transformDerive.cpp"
#include "../headers/sb7-h/vmath.h" //<vmath.h>

#define M_PI     3.14159265358979323846

// Constructor
camera::camera() {
    this->position = vmath::vec3(0.0f, 0.0f,1.0f);
    this->up = vmath::vec3(0.0f, 1.0f, 0.0f);
    this->fov = M_PI/2; // Default camera fov is 90
    this->screen_ratio = 1.0f; // Default assume screen is a square

    this->clip_far = 100.1f;
    this->clip_near = 0.1f;

    // Defaulting the camera to 0 orientation
    this->bank = 0;
    this->pitch = 0;
    this->heading = -M_PI/2.0f;

    // Setting derived values by calling their update methods
    this->updateView();
}

// Setter for position
void camera::setPos(vmath::vec3 new_pos) {
    this->position = new_pos;
    // With position changed, need to update the view
    this->updateView();
}

// Position getter
vmath::vec3 camera::getPos() {
    return this->position;
}

// Updates camera based on window resizing
void camera::setRatio(float new_ratio) {
    this->screen_ratio = new_ratio;
    this->updateView();
}


// Setter for heading
void camera::setHeading(float angle) {
    this->heading = angle;
    this->updateView();
}

// Setter for bank
void camera::setBank(float angle) {
    this->bank = angle;
    this->updateView();
}

// Setter for pitch
void camera::setPitch(float angle) {
    this->pitch = angle;
    this->updateView();
}

// Method for moving the camera forward (positive heading orientation direction)
void camera::moveFoward(float offset) {
    // Get the forward direction relative to heading orientation in 4 dimensions
    vmath::vec4 forward_dir_4d = vmath::vec4(1.0f,0.0f,0.0f,1.0f) * getRotationY(this->heading);
    // Get forward direction in 3 dimensions
    vmath::vec3 forward_dir = vmath::vec3(forward_dir_4d[0],forward_dir_4d[1],forward_dir_4d[2]);
    // Assign new position
    this->position = this->position + offset * forward_dir;
    updateView();
}

// Method for moving the camera left (positive heading orientation direction)
void camera::moveLeft(float offset) {
    // Get the forward direction relative to heading orientation in 4 dimensions
    vmath::vec4 right_dir_4d = vmath::vec4(0.0f,0.0f,1.0f,1.0f) * getRotationY(this->heading);
    // Get forward direction in 3 dimensions
    vmath::vec3 right_dir = vmath::vec3(right_dir_4d[0],right_dir_4d[1],right_dir_4d[2]);
    this->position = this->position - offset * right_dir;
    updateView();
}

// Method for moving the camera up (positive +y direction)
void camera::moveUp(float offset) {
    this->position = this->position + offset * this->up;
    updateView();
}

// Method for turning camera right (or left if negative argument)
void camera::turnRight(float degrees) {
    this->heading = this->heading + degrees;
    // Keeping heading within 2PI and -2PI
    if (this->heading > 2*M_PI) {
        this->heading -= 2*M_PI;
    }
    else if (this->heading < -2*M_PI) {
        this->heading += 2*M_PI;
    }
    updateView();
}

// Method for turning camera up (or down if negative argument)
void camera::turnUp(float degrees) {
    this->pitch = this->pitch - degrees;
    // Keeping heading to be less than 45 degrees to keep things looking fine
    if (this->pitch > M_PI/4.0f) {
        this->pitch = M_PI/4.0f;
    }
    else if (this->pitch < -M_PI/4.0f) {
        this->pitch = -M_PI/4.0f;
    }
    updateView();
}

// Private method, should be used whenever changing position or orientation to update the view matrix for rendering
void camera::updateView() {
    this->up = vmath::vec3(this->abs_up[0],this->abs_up[1],this->abs_up[2]);// * getRotationX(this->pitch);
    // Rotation is a bit different because the main axis is along Z as opposed to X
    vmath::vec4 direction_4d = this->abs_foward * getRotationZ(this->pitch) * getRotationY(this->heading);
    
    vmath::vec3 direction = vmath::vec3(direction_4d[0], direction_4d[1], direction_4d[2]);
    
    this->translationProjection = vmath::lookat(this->position, direction + this->position, this->up);
    // Projection removes the translation component
    this->projection = vmath::mat4(
        translationProjection[0],
        translationProjection[1],
        translationProjection[2],
        vmath::vec4(0.0f,0.0f,0.0f,1.0f)
    );
    this->perspective = vmath::perspective(this->fov, this->screen_ratio, this->clip_near, this->clip_far);
}

// Simple getters for view matrices
vmath::mat4 camera::getProjection() {
    return this->projection;
}

// Translation getter
vmath::mat4 camera::getTranslationProjection() {
    return this->translationProjection;
}

// Perspective getter
vmath::mat4 camera::getPerspective() {
    return this->perspective;
}

// Simple setter for fov
void camera::setFOV(float new_fov) {
    this->fov = new_fov;
}

// Simple getter for fov
float camera::getFOV() {
    return this->fov;
}

// near clip getter
float camera::getNearClip() {
    return this->clip_near;
}

// getter function for far clip
float camera::getFarClip() {
    return this->clip_far;
}