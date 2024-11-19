#pragma once
#include"../Dependencies/stb_image.h"
#include<iostream>

struct Texture {


	Texture(int width, int height, int nrChannels, unsigned char* data)
		:Width(width),Height(height),NrChannels(nrChannels),data(data)
	{}

	Texture() = default;

	~Texture() {stbi_image_free(data); }

 

    // Move Assignment Operator
    Texture& operator=(Texture&& other) noexcept {
        if (this == &other) return *this; // Self-assignment check

        // Free existing data
        if (data) {
            stbi_image_free(data);
        }

        // Transfer ownership
        Width = other.Width;
        Height = other.Height;
        NrChannels = other.NrChannels;
        data = other.data;

        other.data = nullptr;
        other.Width = 0;
        other.Height = 0;
        other.NrChannels = 0;

        return *this;
    }


	int Width, Height, NrChannels;
	unsigned char* data;
};