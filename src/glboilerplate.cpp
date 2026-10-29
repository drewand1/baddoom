#include "glboilerplate.h"

unsigned int mkShader(GLenum type, const char* src) {
	unsigned int shader = glCreateShader(type);
	glShaderSource(shader, 1, &src, nullptr);
	glCompileShader(shader);

	int success;
	char info[512];
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(shader, 512, nullptr, info);
		std::cerr << "[SHADER COMPILATION] " << info << std::endl;
	}

	return shader;
}

unsigned int mkShaderProgram(const char* vtx, const char* frag) {
	unsigned int vtxShader = mkShader(GL_VERTEX_SHADER, vtx);
	unsigned int fragShader = mkShader(GL_FRAGMENT_SHADER, frag);
	unsigned int p = glCreateProgram();

	glAttachShader(p, vtxShader);
	glAttachShader(p, fragShader);
	glLinkProgram(p);

	int success;
	char info[512];
	glGetProgramiv(p, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(p, 512, nullptr, info);
		std::cerr << "[PROGRAM LINK] " << info << std::endl;
	}

	glDeleteShader(vtxShader);
	glDeleteShader(fragShader);
	return p;
}