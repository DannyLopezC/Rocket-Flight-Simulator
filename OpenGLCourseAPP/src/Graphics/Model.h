#pragma once

#include <iostream>
#include <vector>
#include <string>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Mesh.h"
#include "Texture.h"

class Model
{
public:
	Model();

	void loadModel(const std::string& fileName, const std::string& texturesFileName = "");
	void renderModel();
	void clearModel();

	~Model();
private:

	void loadNode(aiNode* node, const aiScene* scene);
	void loadMesh(aiMesh* mesh, const aiScene* scene);
	void loadMaterials(const aiScene* scene, const std::string& texturesFileName);

	std::vector<Mesh*> meshList;
	std::vector<Texture*> textureList;
	std::vector<unsigned int> meshToTex;
};

