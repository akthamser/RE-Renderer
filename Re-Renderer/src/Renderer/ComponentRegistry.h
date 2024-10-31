#pragma once
#include<vector>
#include<string>
#include<unordered_map>
#include<memory>
#include<iostream>
#include"Entity.h"
#include"../config.h"

namespace Re_Renderer {


	template< typename T>
	class ComponentRegistry {

	public:

		ComponentRegistry(size_t initial_capacity, float growth_factor);
		~ComponentRegistry() = default;


		template<typename... Arg>
		void emplace_back(EntID id, Arg&&... args);
		void remove(EntID id);

		T* getComponent(EntID id);

		bool contains(EntID id) const;
		size_t size() const;
		size_t capacity() const;
		void reserve(size_t capacity);
		void grow();

		std::vector<EntID>& getEntitiesIds();
		std::vector<T>& getAllComponents();
			
	private:

		std::vector<T> m_Components;
		std::vector<EntID> m_Entities;
		std::unordered_map<EntID, size_t> m_Map;
		float m_GrowthFactor;
	};

	template<typename T>
	ComponentRegistry<T>::ComponentRegistry(size_t initial_capacity, float growth_factor)
				: m_GrowthFactor(growth_factor){
		reserve(initial_capacity);
	}


	template<typename T>
	template<typename... Arg>
	void ComponentRegistry<T>::emplace_back(EntID id, Arg&&... args) {
		
		auto it = m_Map.find(id);
		if (it != m_Map.end())
			return;

		if (size() >= capacity() ) {
			grow();
		}

		m_Map[id] = m_Components.size();
		m_Components.emplace_back(std::forward<Arg>(args)...);
		m_Entities.emplace_back(id);


	}
	
	template<typename T>
	void ComponentRegistry<T>::remove(EntID id) {

		auto it = m_Map.find(id);
		if (it == m_Map.end())
			return;

		size_t index = it->second;

		if (index < m_Components.size() - 1) {

			m_Components[index] = std::move(m_Components.back());
			m_Entities[index] = std::move(m_Entities.back());

			m_Map[m_Entities[index]] = index;

		}

		m_Components.pop_back();
		m_Entities.pop_back();
		m_Map.erase(id);

	};

	template<typename T>
	T* ComponentRegistry<T>::getComponent(EntID id) {
		
		auto it = m_Map.find(id);
		if (it == m_Map.end())
			return nullptr;

		size_t index = it->second;
		return &m_Components[index];
	};



	template<typename T>
	std::vector<EntID>& ComponentRegistry<T>::getEntitiesIds() { return m_Entities; };

	template<typename T>
	std::vector<T>& ComponentRegistry<T>::getAllComponents() { return m_Components; };

	template<typename T>
	bool ComponentRegistry<T>::contains(EntID id) const {
		return m_Map.find(id) != m_Map.end();
	}


	template<typename T>
	size_t ComponentRegistry<T>::size() const {
		return m_Components.size();
	}
	template<typename T>
	size_t ComponentRegistry<T>::capacity() const {
		return m_Components.capacity();
	}

	template<typename T>
	void ComponentRegistry<T>::reserve(size_t capacity) {
		m_Components.reserve(capacity);
		m_Entities.reserve(capacity);
	}	
	template<typename T>
	void ComponentRegistry<T>::grow() {

		size_t newCapacity = static_cast<size_t>(m_Components.capacity() * m_GrowthFactor);
		m_Components.reserve(newCapacity);
		m_Entities.reserve(newCapacity);
	}


}