#pragma once
#include<vector>
#include<string>
#include<unordered_map>
#include<memory>
#include<iostream>
#include"../config.h"
#include"Entity.h"
#include<optional>
#include"SparseSet.h"


namespace Re_Renderer {



	class Scene {

	public:
		std::vector<Entity> Entites;
		std::unordered_map<EntID, int> EntityMap;

		Scene() = default;
		EntID CreateEntity(std::string name = "", EntID parentID = 0);
		void RemoveEntity(EntID entityID);
		void makeChild(EntID id, EntID parentId);
		void PrintHierarchy();
		void PrintEntity(Entity* entity, int depth = 0);

		Entity* getEntityByID(const EntID& id);

	private:
		EntID currentID = 1;


	};



}