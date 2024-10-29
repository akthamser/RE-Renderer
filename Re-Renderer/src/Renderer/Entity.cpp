#include"Entity.h"



Entity::Entity(unsigned int id, std::string name, unsigned int parentID)
	:ID(id), ParentID(parentID), Name(name)
{
}