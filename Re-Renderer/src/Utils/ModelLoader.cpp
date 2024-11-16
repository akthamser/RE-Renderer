#include"ModelLoader.h"
#include"../Renderer/ShaderType.h"
#include"../Renderer/AssetsManager.h"

namespace Re_Renderer{



Model ModelLoader::loadNewModel(const std::string& path,bool flip) {

	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
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

    processNode(scene->mRootNode, scene, &model, directory,flip);


};

size_t ModelLoader::countNodes(const aiNode* node) {
    if (!node) return 0;

    size_t count = 1; // Count the current node

    for (unsigned int i = 0; i < node->mNumChildren; ++i) {
        count += countNodes(node->mChildren[i]); // Add the children recursively
    }

    return count;
}


size_t ModelLoader::processNode(aiNode* ai_node, const aiScene* scene, Model* model, const std::string& directory,bool flip) {

    size_t index = model->nodes.size();
    Node& node = model->nodes.emplace_back();
    node.material = -1;
    node.mesh = -1;
    node.name = ai_node->mName.C_Str();

    for (int i = 0 ; i < ai_node->mNumMeshes ; i++) {

        aiMesh* aimesh = scene->mMeshes[ai_node->mMeshes[i]];
        
        node.children.push_back(model->nodes.size());
        Node& mnode = model->nodes.emplace_back();
        mnode.name = aimesh->mName.C_Str();

        mnode.mesh = model->meshes.size();
        processMesh(aimesh,mnode, scene, model, directory,flip);

    }


    for (int i = 0; i < ai_node->mNumChildren; i++) {
        
        size_t childeIndex = processNode(ai_node->mChildren[i], scene, model, directory,flip);
        node.children.push_back(childeIndex);
    }

    return index;

}


void ModelLoader::processMesh(aiMesh* ai_mesh,Node& node, const aiScene* scene, Model* model, const std::string& directory,bool flip) {

    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    for (int i = 0;i < ai_mesh->mNumVertices;i++) {
    
        Vertex vertex;
        glm::vec3 pos = glm::vec3(ai_mesh->mVertices[i].x,
                                  ai_mesh->mVertices[i].y, 
                                  ai_mesh->mVertices[i].z);
        vertex.Position = pos;

        glm::vec3 normal = glm::vec3(ai_mesh->mNormals[i].x,
                                      ai_mesh->mNormals[i].y,
                                      ai_mesh->mNormals[i].z);

        vertex.Normal = normal;


        if (ai_mesh->mTextureCoords)
        {
            glm::vec2 textCoord;
            textCoord.x = ai_mesh->mTextureCoords[0][i].x;
            textCoord.y = ai_mesh->mTextureCoords[0][i].y;
            vertex.TexCoords = textCoord;
        }
        else
            vertex.TexCoords = glm::vec2(0.0f, 0.0f);

        if (ai_mesh->mTangents)
            vertex.Tangent = glm::vec3(ai_mesh->mTangents[i].x, ai_mesh->mTangents[i].y, ai_mesh->mTangents[i].z);

        if (ai_mesh->mBitangents)
            vertex.Bitangent = glm::vec3(ai_mesh->mBitangents[i].x, ai_mesh->mBitangents[i].y, ai_mesh->mBitangents[i].z);


        vertices.push_back(vertex);
    }

    for (int i = 0;i < ai_mesh->mNumFaces;i++) {

        aiFace face = ai_mesh->mFaces[i];
        for (int j = 0; j < face.mNumIndices; j++) {
            indices.push_back(face.mIndices[j]);
        }

    }

    model->meshes.emplace_back(vertices,indices);

    if (ai_mesh->mMaterialIndex >= 0) {

        node.material = model->materials.size();
        processMaterial(scene->mMaterials[ai_mesh->mMaterialIndex],node, scene, model, directory,flip);
    }

}


void ModelLoader::processMaterial(aiMaterial* ai_mat, Node& node, const aiScene* scene, Model* model, const std::string& directory,bool flip) {

    
    
    aiString localpath;
    Components::Material material(ShaderType::Blin_Phong);
    //diffuse
    if (ai_mat->GetTextureCount(aiTextureType_DIFFUSE)) {
        ai_mat->GetTexture(aiTextureType_DIFFUSE, 0, &localpath);
        std::string fullPath = directory + "/" + localpath.C_Str();

        material.Diffuse = &m_AssetsManager.loadTexture(fullPath,flip);
    }

    //normal map
    if (ai_mat->GetTextureCount(aiTextureType_NORMALS)) {
        ai_mat->GetTexture(aiTextureType_NORMALS, 0, &localpath);
        std::string fullPath = directory + "/" + localpath.C_Str();


        material.NormalMap = &m_AssetsManager.loadTexture(fullPath, flip);
    }


    //speculer map
    if (ai_mat->GetTextureCount(aiTextureType_SPECULAR)) {
        ai_mat->GetTexture(aiTextureType_SPECULAR, 0, &localpath); 
        std::string fullPath = directory + "/" + localpath.C_Str();

        material.SpecularMap = &m_AssetsManager.loadTexture(fullPath, flip);

    }

    //ambient
    if (ai_mat->GetTextureCount(aiTextureType_AMBIENT)) {
        ai_mat->GetTexture(aiTextureType_AMBIENT, 0, &localpath);
        std::string fullPath = directory + "/" + localpath.C_Str();


        material.Ambient = &m_AssetsManager.loadTexture(fullPath, flip);
    }

    model->materials.push_back(material);
   
}
}