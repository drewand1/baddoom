#ifndef KE_MODEL_H
#define KE_MODEL_H

#include <vector>
#include <glad/glad.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <stb_image.h>
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <iostream>
#include <fstream>
#include "vec.hpp"
#include "sparse.h"

namespace kE {

struct Vertex {
	float x, y, z, u, v;
};

struct Instance {
	glm::mat4 transform = glm::mat4(1.0f);
};

struct Mesh {
	unsigned int vao = 0, vbo = 0, ebo = 0, texture = 0, nIndices = 0;
};

struct Model {
	std::vector<Mesh> meshes;
	kE::SparseSet<Instance> instanceData;
	unsigned int ibo = 0;
	bool reuploadToGPUOnDraw = true;

	explicit Model(int cap) : instanceData(cap) {}
};

unsigned int mkTexture(const char* file);

Mesh processMesh(const aiScene* scene, const aiMesh* assimpMesh, const Model& model, const std::string& rscDir);

bool loadObj(Model& model, const std::string& file, const std::string& rscDir);

bool loadCum(Model& model, const std::string& file, const std::string& rscDir);

glm::mat4 makeTransformMatrix(glm::vec3 pos, glm::vec3 scale, glm::vec3 rot);

glm::mat4 makeTransformMatrix(Vec3f pos, Vec3f scale, Vec3f rot);


}

#endif