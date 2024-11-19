#pragma once


#include<iostream>
#include"../Dependencies/stb_image.h"
#include"../Engine/Texture.h"

namespace Re_Renderer {

	class AssetsManager;


class TextureLoader {

public:
	TextureLoader(AssetsManager& assetsManager) :m_AssetsManager(assetsManager) {};
	Texture loadNewTexture(const std::string& path, bool flip);

private:
	AssetsManager& m_AssetsManager;


};



};