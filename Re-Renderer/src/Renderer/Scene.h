#pragma once
#include<vector>
#include<string>
#include<unordered_map>
#include<memory>
#include"Entity.h"
#include<iostream>

class Scene {

public:
	std::vector<Entity> Entites;
	std::unordered_map<unsigned int, int> EntityMap;
	Scene() = default;
	unsigned int CreateEntity(std::string name = "", unsigned int parentID = 0);
	void RemoveEntity(unsigned int entityID);
	void makeChild(int id,int parentId);
	void PrintHierarchy();
	void PrintEntity(int id , int depth = 0);
private:
	unsigned int currentID = 1;


};