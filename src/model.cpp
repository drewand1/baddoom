#include "model.h"

using namespace kE;

extern unsigned int noTex;


unsigned int kE::mkTexture(const char* file) {
	unsigned int tex;
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	int w, h, nChannels;
	unsigned char* data = stbi_load(file, &w, &h, &nChannels, 0);
	if (data == nullptr) {
		std::cerr << "[MAKE TEXTURE] Failed to load texture." << std::endl;
		return noTex;
	}

	if (nChannels == 3) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	} else if (nChannels == 4) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	}
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(data);
	return tex;
}

Mesh kE::processMesh(const aiScene* scene, const aiMesh* assimpMesh, const Model& model, const std::string& rscDir) {
	Mesh mesh;
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;

	// Get texture
	const aiMaterial* material = scene->mMaterials[assimpMesh->mMaterialIndex];

	if (assimpMesh->mMaterialIndex < scene->mNumMaterials && material != nullptr) {
		aiString file;
		if (material->GetTexture(aiTextureType_DIFFUSE, 0, &file) == AI_SUCCESS) {
			mesh.texture = mkTexture((rscDir + file.C_Str()).c_str());
		} else {
			mesh.texture = noTex;
		}
	} else {
		mesh.texture = noTex;
	}

	// Put in vertex data
	vertices.reserve(assimpMesh->mNumVertices);

	int uvIdx = 0;
	if (material && material->Get(AI_MATKEY_UVWSRC(aiTextureType_DIFFUSE, 0), uvIdx) != AI_SUCCESS)
		uvIdx = 0;

	bool hasUV = assimpMesh->HasTextureCoords(uvIdx);

	for (int i = 0; i < assimpMesh->mNumVertices; i++) {
		aiVector3D assimpV = assimpMesh->mVertices[i];
		vertices.push_back({assimpV.x, assimpV.y, assimpV.z, 0, 0});
		if (hasUV) {
			aiVector3D uv = assimpMesh->mTextureCoords[uvIdx][i];
			vertices[vertices.size() - 1].u = uv.x;
			vertices[vertices.size() - 1].v = uv.y;
		}
	}

	// Put in index data
	indices.reserve(assimpMesh->mNumFaces * 3);

	for (int i = 0; i < assimpMesh->mNumFaces; i++) {
		if (assimpMesh->mFaces[i].mNumIndices != 3)
			continue;
		indices.push_back(assimpMesh->mFaces[i].mIndices[0]);
		indices.push_back(assimpMesh->mFaces[i].mIndices[1]);
		indices.push_back(assimpMesh->mFaces[i].mIndices[2]);
	}

	mesh.nIndices = indices.size();

	// Set up GL buffer objects
	glGenVertexArrays(1, &mesh.vao);
	glBindVertexArray(mesh.vao);
	glGenBuffers(1, &mesh.vbo);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
	glGenBuffers(1, &mesh.ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(float) * 5, (void*) 0);
	glEnableVertexArrayAttrib(mesh.vao, 0);
	glVertexAttribPointer(1, 2, GL_FLOAT, false, sizeof(float) * 5, (void*) (sizeof(float) * 3));
	glEnableVertexArrayAttrib(mesh.vao, 1);
	
	glBindBuffer(GL_ARRAY_BUFFER, model.ibo);
	glVertexAttribPointer(2, 4, GL_FLOAT, false, sizeof(glm::mat4), (void*) 0);
	glEnableVertexArrayAttrib(mesh.vao, 2);
	glVertexAttribPointer(3, 4, GL_FLOAT, false, sizeof(glm::mat4), (void*) (sizeof(float) * 4));
	glEnableVertexArrayAttrib(mesh.vao, 3);
	glVertexAttribPointer(4, 4, GL_FLOAT, false, sizeof(glm::mat4), (void*) (sizeof(float) * 8));
	glEnableVertexArrayAttrib(mesh.vao, 4);
	glVertexAttribPointer(5, 4, GL_FLOAT, false, sizeof(glm::mat4), (void*) (sizeof(float) * 12));
	glEnableVertexArrayAttrib(mesh.vao, 5);
	glVertexAttribDivisor(2, 1);
	glVertexAttribDivisor(3, 1);
	glVertexAttribDivisor(4, 1);
	glVertexAttribDivisor(5, 1);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	return mesh;
}

bool kE::loadObj(Model& model, const std::string& file, const std::string& rscDir) {

	const aiScene* scene = aiImportFile(file.c_str(),
		aiProcess_Triangulate
		| aiProcess_FlipUVs
	);

	if (!scene) {
		std::cerr << "[LOAD OBJ] Failed to load model " << file << std::endl;
		return false;
	}

	glGenBuffers(1, &model.ibo);

	for (int i = 0; i < scene->mNumMeshes; i++) {
		model.meshes.push_back(processMesh(scene, scene->mMeshes[i], model, rscDir));
	}

	return true;
}

bool kE::loadCum(Model& model, const std::string& file, const std::string& rscDir) {
	std::ifstream incumming;
	incumming.open(file, std::ios::binary);
	if (!incumming.is_open())
		return false;
	
	char signature[12];
	incumming.read(signature, 12);
	if (strncmp(signature, "assimpsucks", 12) != 0)
		return false;

	glGenBuffers(1, &model.ibo);

	char metaBuf[8];
	char diffusePath[128];

	incumming.read(metaBuf, sizeof(unsigned int));
	unsigned int nMeshes = *(reinterpret_cast<unsigned int*>(metaBuf));

	for (int i = 0; i < nMeshes; i++) {
		Mesh mesh;

		// Loading texture
		incumming.read(metaBuf, sizeof(uint16_t));
		uint16_t dpLen = *(reinterpret_cast<uint16_t*>(metaBuf));

		if (dpLen > 0) {
			memset(diffusePath, 0, 128);
			incumming.read(diffusePath, dpLen);
			mesh.texture = mkTexture((rscDir + diffusePath).c_str());
		} else {
			mesh.texture = noTex;
		}


		// Loading vertices and indices
		incumming.read(metaBuf, sizeof(uint64_t));
		uint64_t vertDataSize = *(reinterpret_cast<uint64_t*>(metaBuf));
		char* vertData = new char[vertDataSize];
		incumming.read(vertData, vertDataSize);

		incumming.read(metaBuf, sizeof(uint64_t));
		uint64_t idxDataSize = *(reinterpret_cast<uint64_t*>(metaBuf));
		char* idxData = new char[idxDataSize];
		incumming.read(idxData, idxDataSize);
		mesh.nIndices = idxDataSize / sizeof(uint32_t);

		// Setting up VAO, VBO, and EBO
		glGenVertexArrays(1, &mesh.vao);
		glBindVertexArray(mesh.vao);
		glGenBuffers(1, &mesh.vbo);
		glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
		glBufferData(GL_ARRAY_BUFFER, vertDataSize, vertData, GL_STATIC_DRAW);
		glGenBuffers(1, &mesh.ebo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, idxDataSize, idxData,  GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(float) * 5, (void*) 0);
		glEnableVertexArrayAttrib(mesh.vao, 0);
		glVertexAttribPointer(1, 2, GL_FLOAT, false, sizeof(float) * 5, (void*) (sizeof(float) * 3));
		glEnableVertexArrayAttrib(mesh.vao, 1);
		
		glBindBuffer(GL_ARRAY_BUFFER, model.ibo);
		glVertexAttribPointer(2, 4, GL_FLOAT, false, sizeof(glm::mat4), (void*) 0);
		glEnableVertexArrayAttrib(mesh.vao, 2);
		glVertexAttribPointer(3, 4, GL_FLOAT, false, sizeof(glm::mat4), (void*) (sizeof(float) * 4));
		glEnableVertexArrayAttrib(mesh.vao, 3);
		glVertexAttribPointer(4, 4, GL_FLOAT, false, sizeof(glm::mat4), (void*) (sizeof(float) * 8));
		glEnableVertexArrayAttrib(mesh.vao, 4);
		glVertexAttribPointer(5, 4, GL_FLOAT, false, sizeof(glm::mat4), (void*) (sizeof(float) * 12));
		glEnableVertexArrayAttrib(mesh.vao, 5);
		glVertexAttribDivisor(2, 1);
		glVertexAttribDivisor(3, 1);
		glVertexAttribDivisor(4, 1);
		glVertexAttribDivisor(5, 1);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		// Freeing loaded data
		delete [] vertData;
		delete [] idxData;

		model.meshes.push_back(mesh);
	}

	incumming.close();
	return true;
}

glm::mat4 kE::makeTransformMatrix(glm::vec3 pos, glm::vec3 scale, glm::vec3 rot) {
	glm::quat quat(rot);
	glm::mat4 mtx(1.0f);
	mtx = glm::translate(mtx, pos);
	mtx = glm::rotate(mtx, glm::angle(quat), glm::axis(quat));
	mtx = glm::scale(mtx, scale);
	return mtx;
}

glm::mat4 kE::makeTransformMatrix(Vec3f pos, Vec3f scale, Vec3f rot) {
	return makeTransformMatrix(
		glm::vec3(pos.x, pos.y, pos.z),
		glm::vec3(scale.x, scale.y, scale.z),
		glm::vec3(rot.x, rot.y, rot.z)
	);
}
