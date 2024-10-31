#include"Scene.h"


namespace Re_Renderer {


Entity* Scene::getEntityByID(const EntID& id) {

	auto it = EntityMap.find(id);
	if (it == EntityMap.end())
		return nullptr;

	return &Entites[it->second];
}
EntID Scene::CreateEntity(std::string name , EntID parentID ) {

	if (parentID != NullEntID)
	{
		Entity* parent = getEntityByID(parentID);
		if (parent == nullptr) {
			std::cout << "ERROR : CREATING ENTITY : Parent ID Not Found \n" << std::endl;
			parentID = NullEntID;
		}
		else{
			parent->addChild(currentID); // add the new Entity as a child in the Parent Entity
		}

	}

	Entites.emplace_back(currentID,name==""? "Entity " + std::to_string(currentID) : name, parentID,this);
	EntityMap[currentID] = Entites.size() - 1;
	currentID++;
	return currentID - 1;
};


void Scene::RemoveEntity(EntID entityID) {

	auto it = EntityMap.find(entityID);
	if (it == EntityMap.end())
		return;

	int index = it->second;
	if (index < Entites.size() - 1) {
		Entites[index] = Entites.back();
		EntityMap[Entites[index].getID()] = index;
	}

	Entites.pop_back();
	EntityMap.erase(entityID);
};

void Scene::makeChild(EntID id, EntID parentId) {
	
	auto it = EntityMap.find(id);
	if (it == EntityMap.end())
		return;

	int index = it->second;

	it = EntityMap.find(parentId);
	if (parentId != NullEntID && it==EntityMap.end())
		return;



	int currentParentId = Entites[index].getParentID();
	if (currentParentId != NullEntID)
	{
		Entites[EntityMap[currentParentId]].removeChild(id);

	}

	Entites[index].setParentID(parentId);
	if(parentId != NullEntID)
	Entites[it->second].addChild(id);
	
}

void Scene::PrintHierarchy() {
	
	

	for (Entity& entity : Entites) {
		if (entity.getParentID() == NullEntID)
		{
			PrintEntity(&entity);
			
		}	
	}
}

void Scene::PrintEntity(Entity* entity, int depth) {

	std::string space(3 * depth, ' ');


	std::cout << space  << *entity << std::endl;

	
	for (int i = 0; i < entity->getChildrenIDs().size();i++) {
		
		
		PrintEntity(getEntityByID(entity->getChild(i)), depth + 1);
	}


}



}