#include"Entity.h"
#include"Scene.h"



namespace Re_Renderer {


Entity::Entity(EntID id, std::string name, EntID parentID, Scene* scene)
	:m_ID(id), m_ParentID(parentID), m_Name(name),m_Scene(scene)
{
}
//getters
const std::string& Entity::getName() const { return m_Name; };

const EntID& Entity::getID() const { return m_ID; };

const EntID& Entity::getParentID() const { return m_ParentID; };

const std::vector<EntID>& Entity::getChildrenIDs() const { return m_ChildrenIDs; };

const EntID& Entity::getChild(int i) { return m_ChildrenIDs[i]; };


ComponentsHandler& Entity::componentsHundler() { return m_Scene->Components; };
//setters

void Entity::setName(std::string& name) { m_Name = name; };

void Entity::setID(EntID& id) { m_ID = id; };

void Entity::setParentID(EntID& parentId) { m_ParentID = parentId; };

void Entity::addChild(EntID id) { m_ChildrenIDs.push_back(id); };

void Entity::removeChild(const EntID& id) { m_ChildrenIDs.erase(std::remove(m_ChildrenIDs.begin(), m_ChildrenIDs.end(), id), m_ChildrenIDs.end()); };

std::ostream& operator<<(std::ostream& os, const Entity& entity) {
	os << "Entity ID: " << entity.m_ID << ", Name: " << entity.m_Name;
	return os;
}




}