#pragma once
#include<vector>
#include<string>

class Entity {

public:
	Entity() = delete;
	Entity(unsigned int id , unsigned int parent );
	Entity(unsigned int id ,std::string name, unsigned int parent );
	std::string Name;
	unsigned int ID,ParentID;
	std::vector<unsigned int> ChildrenIDs;


};