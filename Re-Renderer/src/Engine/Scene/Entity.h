#pragma once
#include<iostream>
#include<vector>
#include<string>
#include"../../config.h"
#include"../ComponentRegistry.h"
#include"../ComponentsHandler.h"



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

		const EntID& getParentID() ;
		void setParentID(EntID& parentId);

		const std::vector<EntID>& getChildrenIDs() ;
		const EntID& getChild(int i);
		void addChild(EntID id);
		void removeChild(const EntID& id);
		void addIDToChildrenList(const EntID& id);

		ComponentsHandler& componentsHundler();

		template<typename T, typename... Arg>
		T* addComponent(Arg&&... args);

		template<typename T>
		T* getComponent();

		template<typename T>
		void removeComponent();


		friend std::ostream& operator<<(std::ostream& os, const Entity& entity);
	private:
		std::string m_Name;
		EntID m_ID;
		Scene* m_Scene;
	};





	template<typename T, typename... Arg>
	T* Entity::addComponent(Arg&&... args) {

		std::shared_ptr<ComponentRegistry<T>> registry = componentsHundler().getRegistry<T>();
		T* comp = registry->emplace(m_ID, std::forward<Arg>(args)...);
		return comp;
	};

	template<typename T>
	T* Entity::getComponent() {
		T* comp = (componentsHundler().getRegistry<T>())->getComponent(m_ID);
		return comp;
	};

	template<typename T>
	void Entity::removeComponent() {

		(componentsHundler().getRegistry<T>())->remove(m_ID);
	};

}