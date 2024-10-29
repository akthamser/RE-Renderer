#include"Scene.h"


//TO do assigne the childrens if the parent isnt 0

unsigned int Scene::CreateEntity(std::string name , unsigned int parentID ) {

	if (parentID != 0)
	{
		auto it = EntityMap.find(parentID);
		if (it == EntityMap.end()) {
			std::cout << "ERROR : CREATING ENTITY : Parent ID Not Found \n" << std::endl;
			parentID = 0;
		}
		else{
			Entites[it->second].ChildrenIDs.push_back(currentID); // add the new Entity as a child in the Parent Entity
		}

	}

	Entites.emplace_back(currentID,name==""? "Entity " + std::to_string(currentID) : name, parentID);
	EntityMap[currentID] = Entites.size() - 1;
	currentID++;
	return currentID - 1;
};


void Scene::RemoveEntity(unsigned int entityID) {

	auto it = EntityMap.find(entityID);
	if (it == EntityMap.end())
		return;

	int index = it->second;
	if (index < Entites.size() - 1) {
		Entites[index] = Entites.back();
		EntityMap[Entites[index].ID] = index;
	}

	Entites.pop_back();
	EntityMap.erase(entityID);
};

void Scene::PrintEntities() const {
	for (const auto& entity : Entites) {
		std::cout << "Entity ID: " << entity.ID
			<< ", Name: " << entity.Name
			<< ", Parent ID: " << entity.ParentID << std::endl;
		for (int i=0 ; i<entity.ChildrenIDs.size() ; i++)
			std::cout << "Child "<< i << "Id : " << entity.ChildrenIDs[i] << std::endl;
	}
}