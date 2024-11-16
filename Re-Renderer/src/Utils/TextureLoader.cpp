#include"TextureLoader.h"
#include"../Renderer/AssetsManager.h"


namespace Re_Renderer {

Texture TextureLoader::loadNewTexture(const std::string& path, bool flip) {

	stbi_set_flip_vertically_on_load(flip);
	int width, height, nrComponents;
	unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrComponents, 0); 
	return Texture(width, height, nrComponents,data);
};


}