#pragma once
#include"Texture.h"
#include<unordered_map>
#include"Model.h"
#include"../Utils/ModelLoader.h"
#include"../Utils/TextureLoader.h"

namespace Re_Renderer {


class AssetsManager {


public:
	AssetsManager():
		m_ModelLoader(*this),
		m_TextureLoader(*this)
	{	
	}
	std::unordered_map<std::string, Model> m_ModelsCache;
	std::unordered_map<std::string, Texture> m_TexturesCache;


	Texture& loadTexture(const std::string& path,bool flip);
	Model& loadModel(const std::string& path,bool flip);

private:
	ModelLoader m_ModelLoader;
	TextureLoader m_TextureLoader;

};


}