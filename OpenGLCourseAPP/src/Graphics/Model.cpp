#include "Model.h"
#include <fstream>

Model::Model()
{
}

void Model::loadModel(const std::string& fileName, const std::string& texturesFileName)
{
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(fileName,
        aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenSmoothNormals | aiProcess_JoinIdenticalVertices);

    if (!scene) {
        printf("Model (%s) failed to load: %s\n", fileName.c_str(), importer.GetErrorString());
        return;
    }

    printf("charging model %s\n", fileName.c_str());

    loadNode(scene->mRootNode, scene);
    loadMaterials(scene, texturesFileName);
}

void Model::renderModel()
{
    for (size_t i = 0; i < meshList.size(); i++)
    {
        unsigned int materialIndex = meshToTex[i];

        if (materialIndex < textureList.size() && textureList[materialIndex]) {
            textureList[materialIndex]->useTexture();
        }

        meshList[i]->renderMesh();
    }
}

void Model::clearModel()
{
    for (size_t i = 0; i < meshList.size(); i++)
    {
        if (meshList[i]) {
            delete meshList[i];
            meshList[i] = nullptr;
        }
    }

    for (size_t i = 0; i < textureList.size(); i++)
    {
        if (textureList[i]) {
            delete textureList[i];
            textureList[i] = nullptr;
        }
    }
}

void Model::loadNode(aiNode* node, const aiScene* scene)
{
    for (size_t i = 0; i < node->mNumMeshes; i++)
    {
        loadMesh(scene->mMeshes[node->mMeshes[i]], scene);
    }

    for (size_t i = 0; i < node->mNumChildren; i++)
    {
        loadNode(node->mChildren[i], scene);
    }
}

void Model::loadMesh(aiMesh* mesh, const aiScene* scene)
{
    std::vector<GLfloat> vertices;
    std::vector<unsigned int> indices;

    for (size_t i = 0; i < mesh->mNumVertices; i++)
    {
        vertices.insert(vertices.end(), { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z });
        if (mesh->mTextureCoords[0]) {
            vertices.insert(vertices.end(), { mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y });
        }
        else {
            vertices.insert(vertices.end(), { 0.0f, 0.0f });
        }

        vertices.insert(vertices.end(), { -mesh->mNormals[i].x, -mesh->mNormals[i].y, -mesh->mNormals[i].z });
    }

    for (size_t i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (size_t j = 0; j < face.mNumIndices; j++)
        {
            indices.push_back(face.mIndices[j]);
        }
    }

    Mesh* newMesh = new Mesh();
    newMesh->createMesh(&vertices[0], &indices[0], vertices.size(), indices.size());
    meshList.push_back(newMesh);
    meshToTex.push_back(mesh->mMaterialIndex);
}

void Model::loadMaterials(const aiScene* scene, const std::string& texturesFileName)
{
    textureList.resize(scene->mNumMaterials);

    for (size_t i = 0; i < scene->mNumMaterials; i++)
    {
        aiMaterial* material = scene->mMaterials[i];
        textureList[i] = nullptr;

        if (material->GetTextureCount(aiTextureType_DIFFUSE)) {
            aiString path;
            if (material->GetTexture(aiTextureType_DIFFUSE, 0, &path) == AI_SUCCESS) {
                printf("Material diffuse path: %s\n", path.C_Str());

                if (path.data[0] == '*') {
                    int textureIndex = atoi(path.data + 1);
                    const aiTexture* texture = scene->mTextures[textureIndex];

                    if (texture) {
                        printf("Embedded texture detected: %s\n", path.C_Str());

                        if (texture->mHeight == 0) {
                            std::string tmpFile = "assets/textures/tmp_embedded.png";
                            std::ofstream out(tmpFile, std::ios::binary);
                            out.write((char*)texture->pcData, texture->mWidth);
                            out.close();

                            textureList[i] = new Texture(tmpFile.c_str());
                            textureList[i]->loadTextureA();
                        }
                        else {
                            textureList[i] = new Texture();
                            textureList[i]->createFromData(
                                reinterpret_cast<unsigned char*>(texture->pcData),
                                texture->mWidth,
                                texture->mHeight,
                                GL_RGBA
                            );
                        }
                        continue; // pasa al siguiente material
                    }
                }

                int index = std::string(path.data).rfind("\\");
                std::string fileName = std::string(path.data).substr(index + 1);

                while (true) {
                    size_t pos = fileName.find("//");
                    if (pos == std::string::npos) break;
                    fileName.erase(pos, 2);
                }

                std::string texPath = std::string("assets/textures/" + texturesFileName) + fileName;
                textureList[i] = new Texture(texPath.c_str());

                if (!textureList[i]->loadTexture()) {
                    printf("Failed loading texture at: %s\n", texPath.c_str());
                    delete textureList[i];
                    textureList[i] = nullptr;
                }
            }
        }

        if (!textureList[i]) {
            textureList[i] = new Texture("assets/textures/plain.png");
            textureList[i]->loadTextureA();
        }
    }
}

Model::~Model()
{
    clearModel();
}