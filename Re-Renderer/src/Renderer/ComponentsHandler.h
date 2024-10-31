#pragma once
#include<unordered_map>
#include<typeindex>
#include <type_traits>
#include<memory>
#include"ComponentRegistry.h"


namespace Re_Renderer {


	class ComponentsHandler
	{
	public:
		ComponentsHandler() = default;
		~ComponentsHandler() = default;

		template<typename T>
		std::shared_ptr<ComponentRegistry<T>> getRegistry(size_t initial_capacity = 16, float growth_factor = 2.0f);


	private:
		std::unordered_map<std::type_index, std::shared_ptr<void>> m_ComponentsRegestries;

	};

	template<typename T>
	std::shared_ptr<ComponentRegistry<T>> ComponentsHandler::getRegistry(size_t initial_capacity , float growth_factor) {
			
		auto index = std::type_index(typeid(T));
		auto it = m_ComponentsRegestries.find(index);

		if (it == m_ComponentsRegestries.end()){ //new registry

			m_ComponentsRegestries[index] = std::make_shared<ComponentRegistry<T>>(initial_capacity, growth_factor);

			return std::static_pointer_cast<ComponentRegistry<T>>(m_ComponentsRegestries[index]);

		}
		else{ // registry exists

			return std::static_pointer_cast<ComponentRegistry<T>>(m_ComponentsRegestries[index]);

		}
	
	}


}