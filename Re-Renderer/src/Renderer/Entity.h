#pragma once
#include<iostream>
#include<vector>
#include<string>
#include"../config.h"


namespace Re_Renderer {

	class Scene;

	class Entity {
		
	public:
		Entity() = delete;
		Entity(EntID id, std::string name, EntID parent, Scene* scene);
		~Entity() = default;

		const std::string& getName() const;
		void setName(std::string& name);

		const EntID& getID() const;
		void setID(EntID& id);

		const EntID& getParentID() const;
		void setParentID(EntID& parentId);

		const std::vector<EntID>& getChildrenIDs() const;
		const EntID& getChild(int i);
		void addChild(EntID id);
		void removeChild(const EntID& id);

		friend std::ostream& operator<<(std::ostream& os, const Entity& entity);
	private:
		std::string m_Name;
		EntID m_ID, m_ParentID;
		std::vector<EntID> m_ChildrenIDs;
		Scene* m_Scene;
	};


}