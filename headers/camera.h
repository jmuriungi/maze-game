#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "../headers/sb7-h/sb7.h" //<sb7.h>
#include "../headers/sb7-h/vmath.h" //<vmath.h>

// Class object to encapsulates general camera behavior
class camera {
private:
    // Camera properties for position and orientation
    vmath::vec3 position;
    vmath::vec3 up;    // Direction that is up to the camera's perspective

    float clip_near, clip_far;

    float bank, heading, pitch; // Orientation of the camera in radians
    float fov; // Field of view of the camera
    float screen_ratio;

    vmath::mat4 perspective; // Matrix for storing resulting perspective matrix
    vmath::mat4 projection; // Matrix for storing resulting projection for rendering
    vmath::mat4 translationProjection; // Matrix for storing resulting projection with translation based on camera position rendering (seperate from projection for use with SkyCube)

    // Private methods used by setters to update view transformation matrices when values change
    void updateView();

    // Direction that is taken to be absolute up and forward reference points
    const vmath::vec4 abs_up = vmath::vec4(0.0f, 1.0f, 0.0f, 0.0f);
    const vmath::vec4 abs_foward = vmath::vec4(1.0f, 0.0f, 0.0f, 0.0f);
public:
    // A constructor for camera that simply takes in position
    camera();

    // Simple Setters
    void setPos(vmath::vec3 new_pos);
    void setFOV(float new_pers);
    void setRatio(float new_ratio);

    void setHeading(float new_heading);
    void setBank(float new_bank);
    void setPitch(float new_pitch);

    // Simple Getters
    vmath::vec4 getDirection();
    vmath::vec4 getUp();
    float getFOV();
    vmath::vec3 getPos();

    vmath::mat4 getProjection();
    vmath::mat4 getTranslationProjection();
    vmath::mat4 getPerspective();

    // Camera control methods for changing view //
    // Note: camera currently works somewhat first person perspective, maintaining up in the +y direction

    // Method for turning camera right (or left if negative argument)
    void turnRight(float degrees);
    // Method for turning camera up (or down if negative argument)
    void turnUp(float degrees);
    // Method for moving the camera forward (positive heading orientation direction)
    void moveFoward(float offset);
    // Method for moving the camera left (positive heading orientation direction)
    void moveLeft(float offset);
    // Method for moving the camera up (positive +y direction)
    void moveUp(float offset);

    // Getters for clipping distances
    float getNearClip();
    float getFarClip();

};


#endif
