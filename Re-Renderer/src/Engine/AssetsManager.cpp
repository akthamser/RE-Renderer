#include"AssetsManager.h"

namespace Re_Renderer {

Texture& AssetsManager::loadTexture(const std::string& path, bool flip) {

	auto it = m_TexturesCache.find(path);
	if (it == m_TexturesCache.end())
	{
		m_TexturesCache[path] = m_TextureLoader.loadNewTexture(path, flip);
	}

	return m_TexturesCache[path];

};
Model& AssetsManager::loadModel(const std::string& path, bool flip) {

	auto it = m_ModelsCache.find(path);
	if (it == m_ModelsCache.end())
	{
		auto model = m_ModelLoader.loadNewModel(path,flip);
		m_ModelsCache[path] = model;
	}

	return m_ModelsCache[path];


};
}