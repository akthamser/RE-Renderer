#pragma once
#include"Components/Components.h"
#include"Scene/Entity.h"



namespace Re_Renderer {


struct Node {
	std::string name;
	size_t mesh;
	size_t material;
	std::vector<size_t> children;
};
struct Model {

	std::vector<Node> nodes;
	std::vector<Components::Mesh> meshes;
	std::vector<Components::Material> materials;


};
}