#include"Scene.h"




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

void Scene::makeChild(int id, int parentId) {
	
	auto it = EntityMap.find(id);
	if (it == EntityMap.end())
		return;

	int index = it->second;

	it = EntityMap.find(parentId);
	if (parentId != 0 && it==EntityMap.end())
		return;



	int currentParentId = Entites[index].ParentID;
	if (currentParentId != 0)
	{
		auto& oldChildrens =Entites[EntityMap[currentParentId]].ChildrenIDs;
		oldChildrens.erase(std::remove(oldChildrens.begin(), oldChildrens.end(), id), oldChildrens.end());
	}

	Entites[index].ParentID = parentId;
	if(parentId != 0)
	Entites[it->second].ChildrenIDs.push_back(id);
	
}

void Scene::PrintHierarchy() {
	
	

	for (const auto& entity : Entites) {
		if (entity.ParentID == 0)
		{
			PrintEntity(entity.ID);
			
		}	
	}
}

void Scene::PrintEntity(int id, int depth) {

	std::string space(3 * depth, ' ');

	int index = EntityMap[id];

	std::cout << space  << "Entity ID: " << id << " Name: " << Entites[index].Name << std::endl;

	
	for (int i = 0; i < Entites[index].ChildrenIDs.size();i++) {
		
		PrintEntity(Entites[index].ChildrenIDs[i], depth +1);
	}


}