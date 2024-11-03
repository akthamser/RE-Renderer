#include"Scene.h"


namespace Re_Renderer {


Entity* Scene::getEntityByID(const EntID& id) {

	auto it = EntityMap.find(id);
	if (it == EntityMap.end())
		return nullptr;

	return &Entities[it->second];
}
Entity& Scene::CreateEntity(std::string name , EntID parentID ) {

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

	Entities.emplace_back(currentID,name==""? "Entity " + std::to_string(currentID) : name, parentID,this);
	EntityMap[currentID] = Entities.size() - 1;
	currentID++;
	return Entities.back();
};


void Scene::RemoveEntity(EntID entityID) {

	auto it = EntityMap.find(entityID);
	if (it == EntityMap.end())
		return;

	int index = it->second;
	if (index < Entities.size() - 1) {
		Entities[index] = Entities.back();
		EntityMap[Entities[index].getID()] = index;
	}

	Entities.pop_back();
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



	int currentParentId = Entities[index].getParentID();
	if (currentParentId != NullEntID)
	{
		Entities[EntityMap[currentParentId]].removeChild(id);

	}

	Entities[index].setParentID(parentId);
	if(parentId != NullEntID)
	Entities[it->second].addChild(id);
	
}

void Scene::PrintHierarchy() {
	
	

	for (Entity& entity : Entities) {
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