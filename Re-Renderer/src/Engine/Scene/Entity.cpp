#include"Entity.h"
#include"Scene.h"



namespace Re_Renderer {


Entity::Entity(EntID id, std::string name, EntID parentID, Scene* scene)
	:m_ID(id), m_Name(name),m_Scene(scene)
{
	addComponent<Components::Transform>();
	setParentID(parentID);

}


//getters
const std::string& Entity::getName() const { return m_Name; };

const EntID& Entity::getID() const { return m_ID; };

const EntID& Entity::getParentID()  { return getComponent<Components::Transform>()->m_ParentID; };

const std::vector<EntID>& Entity::getChildrenIDs()  { return getComponent<Components::Transform>()->m_ChildrenIDs; };

const EntID& Entity::getChild(int i) { return getComponent<Components::Transform>()->m_ChildrenIDs[i]; };


ComponentsHandler& Entity::componentsHundler() { return m_Scene->Components; };
//setters

void Entity::setName(std::string& name) { m_Name = name; };

void Entity::setID(EntID& id) { m_ID = id; };

void Entity::setParentID(EntID& parentId) { 

	auto transform = getComponent<Components::Transform>();
	if (transform->m_ParentID == parentId || (parentId != 0 &&  m_Scene->getEntityByID(parentId) == nullptr))
		return;

	if(transform->m_ParentID != 0)
	m_Scene->getEntityByID(transform->m_ParentID)->removeChild(m_ID);

	transform->m_ParentID = parentId;

	if(parentId != 0)
	m_Scene->getEntityByID(parentId)->addIDToChildrenList(m_ID);
};

void Entity::addChild(EntID id) { 
	auto ent = m_Scene->getEntityByID(id);
	if(ent != nullptr)
	ent->setParentID(m_ID);
};

void Entity::removeChild(const EntID& id) { 
	auto transform = getComponent<Components::Transform>();
	transform->m_ChildrenIDs.erase(std::remove(transform->m_ChildrenIDs.begin(), transform->m_ChildrenIDs.end(), id), transform->m_ChildrenIDs.end());

};

void Entity::addIDToChildrenList(const EntID& id) {
	auto transform = getComponent<Components::Transform>();
	transform->m_ChildrenIDs.push_back(id);
};

std::ostream& operator<<(std::ostream& os, const Entity& entity) {
	os << "Entity ID: " << entity.m_ID << ", Name: " << entity.m_Name;
	return os;
}




}