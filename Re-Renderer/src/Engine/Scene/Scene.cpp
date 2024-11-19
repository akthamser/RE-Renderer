#include"Scene.h"
#include"../ComponentsHandler.h"



namespace Re_Renderer {

Scene::Scene() {
	Entities.reserve(1024);
};
Entity* Scene::getEntityByID(const EntID& id) {

	auto it = EntityMap.find(id);
	if (it == EntityMap.end())
		return nullptr;

	return &Entities[it->second];
}
Entity& Scene::CreateEntity(std::string name , EntID parentID ) {


	Entity& entity = Entities.emplace_back(currentID,name==""? "Entity " + std::to_string(currentID) : name, parentID,this);
	EntityMap[currentID] = Entities.size() - 1;
	currentID++;
	return entity;
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

Components::Camera* Scene::getActiveCamera()  { 
	return Components.getRegistry<Components::Camera>()->getComponentByIndex(activeCamera);
};

void Scene::setActiveCamera(size_t i) {

	if (Components.getRegistry<Components::Camera>()->getComponentByIndex(i) != nullptr)
		activeCamera = i;
	
		

};



EntID Scene::CreateModel(const Model& model, EntID parentID ) {

	if (model.nodes.size() == 0)
		return 0;

	const Node& root = model.nodes[0];

	Entity& entity = CreateEntity(root.name, parentID);

	if (root.mesh != -1)
		entity.addComponent<Components::Mesh>(model.meshes[root.mesh]);
	if (root.material != -1)
		entity.addComponent<Components::Material>(model.materials[root.material]);

	EntID entId = entity.getID();

	for (int i = 0; i < root.children.size(); i++) {

		CreateNode(model.nodes[root.children[i]], model, entId);

	}

	return entId;

};


void Scene::CreateNode(const Node& node, const Model& model,EntID parentID) {
	
	Entity& entity = CreateEntity(node.name,parentID);

	if (node.mesh != -1)
		entity.addComponent<Components::Mesh>(model.meshes[node.mesh]);
	if(node.material != -1)
		entity.addComponent<Components::Material>(model.materials[node.material]);

	EntID entId = entity.getID(); 

	for (int i = 0; i < node.children.size(); i++) {
	
	   CreateNode(model.nodes[node.children[i]],model, entId);

	}



};



}