/*

formatobj
Simple utility to take a .obj file and transform it into a list of vertices,
indices, and diffuse texture data pre-formatted to fit baddoom's rendering code
because assimp takes half a second to load a few thousand triangles.

Takes in a .obj file format. Emits a .cum file (conglomerate unwrapped model)
because assimp's genius .assbin file is meant as a temporary caching thing and
therefore will be autodeleted by some utilities. A .cum file isstructured as
follows:

HEADER {
	HEX SIGNATURE:
		0x61 0x73 0x73 0x67 0x6D 0x70 0x73 0x75 0x63 0x6B 0x73 0x0
		or "assimpsucks"

	u32 nMeshes
}

MESH {
	DIFFUSE {
		u16 pathLength
		char relativePath[]
	}
	
	VERTICES {
		u64 dataSize
		data {
			f32 x, y, z, u, v
		}
	}

	INDICES {
		u64 dataSize
		u32 data[]
	}
}

*/

#include <fstream>
#include <iostream>
#include <cstring>
#include <cstdint>
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

using namespace std;

void applyHeader(ofstream& file) {
	file.write("assimpsucks", 12);
}

void objToCum(const aiScene* scene, ofstream& out) {
	out.write(reinterpret_cast<const char*>(&(scene->mNumMeshes)), sizeof(unsigned int));

	for (int i = 0; i < scene->mNumMeshes; i++) {
		const aiMesh* mesh = scene->mMeshes[i];

		// Diffuse texture name
		const aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		aiString file;

		if (material && material->GetTexture(aiTextureType_DIFFUSE, 0, &file) == AI_SUCCESS) {
			int16_t size = strlen(file.C_Str());
			out.write(reinterpret_cast<const char*>(&size), sizeof(int16_t));
			out.write(file.C_Str(), size);
		} else {
			int16_t size = 0;
			out.write(reinterpret_cast<const char*>(&size), sizeof(int16_t));
		}

		// Vertices
		float* vertices = new float[mesh->mNumVertices * 5];	// x, y, z, u, v
		size_t cursor = 0;

		int uvIdx = 0;
		if (material && material->Get(AI_MATKEY_UVWSRC(aiTextureType_DIFFUSE, 0), uvIdx) != AI_SUCCESS)
			uvIdx = 0;

		bool hasUV = mesh->HasTextureCoords(uvIdx);

		for (int i = 0; i < mesh->mNumVertices; i++) {
			aiVector3D vertex = mesh->mVertices[i];
			vertices[cursor] = vertex.x;
			vertices[cursor + 1] = vertex.y;
			vertices[cursor + 2] = vertex.z;

			if (hasUV) {
				aiVector3D uv = mesh->mTextureCoords[uvIdx][i];
				vertices[cursor + 3] = uv.x;
				vertices[cursor + 4] = uv.y;
			} else {
				vertices[cursor + 3] = 0;
				vertices[cursor + 4] = 0;
			}

			cursor += 5;
		}

		uint64_t verticesSize = sizeof(float) * mesh->mNumVertices * 5;
		out.write(reinterpret_cast<const char*>(&verticesSize), sizeof(uint64_t));
		out.write(reinterpret_cast<const char*>(vertices), verticesSize);
		delete [] vertices;

		// Indices
		uint32_t* indices = new uint32_t[mesh->mNumFaces * 3];
		cursor = 0;

		for (int i = 0; i < mesh->mNumFaces; i++) {
			aiFace face = mesh->mFaces[i];
			if (face.mNumIndices != 3)
				continue;
			
			indices[cursor] = face.mIndices[0];
			indices[cursor + 1] = face.mIndices[1];
			indices[cursor + 2] = face.mIndices[2];
			cursor += 3;
		}

		uint64_t indicesSize = sizeof(uint32_t) * mesh->mNumFaces * 3;
		out.write(reinterpret_cast<const char*>(&indicesSize), sizeof(uint64_t));
		out.write(reinterpret_cast<const char*>(indices), indicesSize);
		delete [] indices;
	}
}

int main(int argc, char* argv[]) {
	cerr << "Hi?" << endl;
	char outName[128] = "cum.cum";
	char fileName[128] = "thisFileDoesntExist.";
	bool outFlag = false;
	bool inFileExists = false;

	for (int i = 1; i < argc; i++) {
		cout << argv[i] << endl;

		if (strcmp(argv[i], "-o") == 0) {
			outFlag = true;
			continue;
		}

		if (outFlag) {
			strncpy(outName, argv[i], 128);
			outName[127] = '\0';
			outFlag = false;
		} else if (!inFileExists) {
			strncpy(fileName, argv[i], 128);
			fileName[127] = '\0';
			inFileExists = true;
		} else {
			cerr << "formatobj: Unexpected token " << argv[i] << endl;
			exit(-1);
		}
	}

	if (!inFileExists) {
		cerr << "formatobj: No input file specified." << endl;
		exit(-1);
	}

	const aiScene* scene = aiImportFile(fileName,
		aiProcess_Triangulate
		| aiProcess_FlipUVs
		| aiProcess_JoinIdenticalVertices
	);

	if (!scene) {
		cerr << "formatobj: File " << fileName << " does not exist or could not be loaded." << endl;
		exit(-1);
	}

	ofstream out;
	out.open(outName, ios::binary);

	if (!out.is_open()) {
		cerr << "formatobj: Failed to open output file for writing." << endl;
		exit(-1);
	}

	applyHeader(out);
	objToCum(scene, out);

	out.close();

	return 0;
}
