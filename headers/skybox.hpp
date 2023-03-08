#ifndef _SKYBOX_H_
#define _SKYBOX_H_

#include "../headers/sb7-h/vmath.h"
#include "GL/GL.h"
#include <vector>

class skybox
{
    private:

    public:
        skybox();

        skybox()
        {

        }

        void createCube(std::vector<vmath::vec4> &vertices);

        void subPoints(int a, int b, int c, int d,std::vector<vmath::vec4> &vertices);

        void loadCubeTextures(std::string directory, GLuint texture_ID);

        void loadCubeSide(GLint texture_ID, GLenum side, std::string file);

        unsigned int charToUInt(char * loc);
};

#endif