#pragma once
#include"../Renderer/Entity.h"
#include"../Renderer/Scene.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include"../Renderer/Model.h"
#include<iostream>
#include"../Dependencies/stb_image.h"

namespace Re_Renderer {

class AssetsManager;

class ModelLoader {

public:
	ModelLoader(AssetsManager& assetsManager):m_AssetsManager(assetsManager) {};
	Model loadNewModel(const std::string& path,bool flip);

private:
	AssetsManager& m_AssetsManager;


	size_t processNode(aiNode* ai_node, const aiScene* scene, Model* model, const std::string& directory, bool flip);
	void processMesh(aiMesh* ai_mesh, Node& node, const aiScene* scene, Model* model, const std::string& directory, bool flip);
	void processMaterial(aiMaterial* ai_mat, Node& node, const aiScene* scene, Model* model, const std::string& directory, bool flip);
	size_t countNodes(const aiNode* node);
};
};


