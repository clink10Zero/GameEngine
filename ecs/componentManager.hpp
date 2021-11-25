#pragma once

#include "./ComponentArray.hpp"
#include "./Types.hpp"
#include <any>
#include <memory>
#include <unordered_map>

class ComponentManager
{
	public:
		template<typename T>
		void RegisterComponnent()
		{
			const char* typeName = typeid(T).name();
			assert(mComponentTypes.find(typeName) == mComponentTypes.end() && "Registering componnent type more than once.");

			mComponentTypes.insert({ typeName, mNextComponentType });
			mComponentArrays.insert({typeName, std::make_shared<ComponentArray<T>>()});

			++mNextComponentType;
		}

		template<typename T>
		ComponentType GetComponentType()
		{
			const char* typeName = typeid(T).name();

			assert(mComponentTypes.find(typeName) != mComponentTypes.end() && "Component not registered before use.");

			return mComponentTypes[typeName];
		}

		template<typename T>
		void AddComponent(GameObject go, T component)
		{
			GetComponentArray<T>()->InsertData(go, component);
		}

		template<typename T>
		void RemoveComponent(GameObject go)
		{
			GetComponentArray<T>()->RemoveData(go);
		}

		template<typename T>
		T& GetComponent(GameObject go)
		{
			return GetComponentArray<T>()->GetData(go);
		}

		template<typename T>
		bool HaveComponent(GameObject go)
		{
			return GetComponentArray<T>()->HaveData(go);
		}

		void GameObjectDestroyed(GameObject go)
		{
			for (auto const& pair : mComponentArrays)
			{
				auto const& component = pair.second;

				component->GameObjectDestroyed(go);
			}
		}


	private:
		std::unordered_map<const char*, ComponentType> mComponentTypes{};
		std::unordered_map<const char*, std::shared_ptr<IComponentArray>> mComponentArrays{};
		ComponentType mNextComponentType{};


		template<typename T>
		std::shared_ptr<ComponentArray<T>> GetComponentArray()
		{
			const char* typeName = typeid(T).name();

			assert(mComponentTypes.find(typeName) != mComponentTypes.end() && "Component not registered before use.");

			return std::static_pointer_cast<ComponentArray<T>>(mComponentArrays[typeName]);
		}
};