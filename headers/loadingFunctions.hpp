#ifndef _LOADINGFUNCTIONS_H_
#define _LOADINGFUNCTIONS_H_

#include "GL/GL.h"
// #include <iostream>
#include <fstream>

class loadingFunctions
{
private:

public:
    loadingFunctions();

    loadingFunctions()
    {

    }

    void load_obj(const char* filename, std::vector<vmath::vec4> &vertices, std::vector<vmath::vec2> &uvs, std::vector<vmath::vec4> &normals, GLuint &number);

    unsigned int charToUInt(char * loc);

    std::string pop_substr(std::string &myString, std::string endPattern);

    void load_BMP(std::string file, unsigned char* &texture_data, unsigned int &tWidth, unsigned int &tHeight);
};

#endif