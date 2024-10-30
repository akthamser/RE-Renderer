#pragma once
#include<vector>
#include<string>

class Entity {
	//TO do figure out the how to hundl components "CREATE Transform Componenet"
public:
	Entity() = delete;
	Entity(unsigned int id ,std::string name, unsigned int parent );
	std::string Name;
	unsigned int ID,ParentID;
	std::vector<unsigned int> ChildrenIDs;


};