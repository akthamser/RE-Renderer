#include"ModelLoader.h"
#include"../Renderer/ShaderType.h"
#include"../ECS/AssetsManager.h"
#include <iostream>

namespace Re_Renderer {

    Model ModelLoader::loadNewModel(const std::string& path, bool flip) {

        Assimp::Importer importer;

        const aiScene* scene = importer.ReadFile(path,
            aiProcess_Triangulate |
            aiProcess_FlipUVs |
            aiProcess_GenSmoothNormals |
            aiProcess_CalcTangentSpace);

        std::string directory = path.substr(0, path.find_last_of('/'));
        Model model;

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
            return model;
        }

        auto numNodes = countNodes(scene->mRootNode);
        model.nodes.reserve(numNodes + scene->mNumMeshes);
        model.meshes.reserve(scene->mNumMeshes);
        model.materials.reserve(scene->mNumMaterials);

        processNode(scene->mRootNode, scene, &model, directory, flip);

        return model;
    };

    size_t ModelLoader::countNodes(const aiNode* node) {
        if (!node) return 0;
        size_t count = 1;
        for (unsigned int i = 0; i < node->mNumChildren; ++i) {
            count += countNodes(node->mChildren[i]);
        }
        return count;
    }

    size_t ModelLoader::processNode(aiNode* ai_node, const aiScene* scene, Model* model, const std::string& directory, bool flip) {

        size_t index = model->nodes.size();
        Node& node = model->nodes.emplace_back();
        node.name = ai_node->mName.C_Str();
        node.material = -1;
        node.mesh = -1;


        for (unsigned int i = 0; i < ai_node->mNumMeshes; i++) {
            aiMesh* aimesh = scene->mMeshes[ai_node->mMeshes[i]];

            if (i == 0) {
                // First mesh goes to the current node
                node.mesh = model->meshes.size();
                processMesh(aimesh, node, scene, model, directory, flip);
            }
            else {
                // Additional meshes become children
                size_t childIdx = model->nodes.size();


                Node& childNode = model->nodes.emplace_back();
                childNode.name = std::string(ai_node->mName.C_Str()) + "_mesh_" + std::to_string(i);
                childNode.mesh = model->meshes.size();


                model->nodes[index].children.push_back(childIdx);

                processMesh(aimesh, childNode, scene, model, directory, flip);
            }
        }

        // Process Children Nodes
        for (unsigned int i = 0; i < ai_node->mNumChildren; i++) {
            size_t childIndex = processNode(ai_node->mChildren[i], scene, model, directory, flip);

            model->nodes[index].children.push_back(childIndex);
        }

        return index;
    }

    void ModelLoader::processMesh(aiMesh* ai_mesh, Node& node, const aiScene* scene, Model* model, const std::string& directory, bool flip) {

        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;

        for (unsigned int i = 0; i < ai_mesh->mNumVertices; i++) {

            Vertex vertex;
            vertex.Position.x = ai_mesh->mVertices[i].x;
            vertex.Position.y = ai_mesh->mVertices[i].y;
            vertex.Position.z = ai_mesh->mVertices[i].z;


            if (ai_mesh->HasNormals()) {
                vertex.Normal.x = ai_mesh->mNormals[i].x;
                vertex.Normal.y = ai_mesh->mNormals[i].y;
                vertex.Normal.z = ai_mesh->mNormals[i].z;
            }
            else {
                vertex.Normal = glm::vec3(0.0f, 1.0f, 0.0f); // Fallback
            }

            // Safe UV Access
            if (ai_mesh->HasTextureCoords(0)) {
                vertex.TexCoords.x = ai_mesh->mTextureCoords[0][i].x;
                vertex.TexCoords.y = ai_mesh->mTextureCoords[0][i].y;
            }
            else {
                vertex.TexCoords = glm::vec2(0.0f);
            }

            // Safe Tangent Access
            if (ai_mesh->HasTangentsAndBitangents()) {
                vertex.Tangent = glm::vec3(ai_mesh->mTangents[i].x, ai_mesh->mTangents[i].y, ai_mesh->mTangents[i].z);
                vertex.Bitangent = glm::vec3(ai_mesh->mBitangents[i].x, ai_mesh->mBitangents[i].y, ai_mesh->mBitangents[i].z);
            }
            else {
                vertex.Tangent = glm::vec3(0.0f);
                vertex.Bitangent = glm::vec3(0.0f);
            }

            vertices.push_back(vertex);
        }

        for (unsigned int i = 0; i < ai_mesh->mNumFaces; i++) {
            aiFace face = ai_mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j++) {
                indices.push_back(face.mIndices[j]);
            }
        }

        model->meshes.emplace_back(vertices, indices);

        if (ai_mesh->mMaterialIndex >= 0) {
            node.material = model->materials.size();
            processMaterial(scene->mMaterials[ai_mesh->mMaterialIndex], node, scene, model, directory, flip);
        }
    }

    void ModelLoader::processMaterial(aiMaterial* ai_mat, Node& node, const aiScene* scene, Model* model, const std::string& directory, bool flip) {

        Components::Material material(ShaderType::Blin_Phong);
        aiString localpath;

        // Helper lambda to load texture if it exists
        auto tryLoadTexture = [&](aiTextureType type, Texture** targetPtr) {
            if (ai_mat->GetTextureCount(type) > 0) {
                if (ai_mat->GetTexture(type, 0, &localpath) == AI_SUCCESS) {
                    std::string fullPath = directory + "/" + localpath.C_Str();
                    *targetPtr = &m_AssetsManager.loadTexture(fullPath, flip);
                }
            }
            };

        tryLoadTexture(aiTextureType_DIFFUSE, &material.Diffuse);
        tryLoadTexture(aiTextureType_NORMALS, &material.NormalMap);
        tryLoadTexture(aiTextureType_SPECULAR, &material.SpecularMap);
        tryLoadTexture(aiTextureType_AMBIENT, &material.Ambient);

        // Also load Diffuse Color (for models without textures)
        aiColor3D color(1.f, 1.f, 1.f);
        if (ai_mat->Get(AI_MATKEY_COLOR_DIFFUSE, color) == AI_SUCCESS) {
            material.BaseColor = glm::vec3(color.r, color.g, color.b);
        }

        model->materials.push_back(material);
    }
}