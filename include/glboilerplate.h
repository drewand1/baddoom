#ifndef GLBOILERPLATE_H
#define GLBOILERPLATE_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

unsigned int mkShader(GLenum type, const char* src);

unsigned int mkShaderProgram(const char* vtx, const char* frag);

#endif