#pragma once

#include "Types.hpp"
#include <array>
#include <cassert>
#include <unordered_map>

class IComponentArray
{
	public:
		virtual ~IComponentArray() = default;
		virtual void GameObjectDestroyed(GameObject go) = 0;
};

template<typename T>
class ComponentArray : public IComponentArray
{
	public:
		void InsertData(GameObject go, T component)
		{
			assert(mGameObjectToIndexMap.find(go) == mGameObjectToIndexMap.end() && "Component added to same GameObject more than once.");

			size_t newIndex = mSize;
			mGameObjectToIndexMap[go] = newIndex;
			mIndexToGameObjectMap[newIndex] = go;
			mComponentArray[newIndex] = component;
			++mSize;
		}

		void RemoveData(GameObject go)
		{
			assert(mGameObjectToIndexMap.find(go) != mGameObjectToIndexMap.end() && "Remove non-existant componnent");

			size_t indexOfRemovedGameObject = mGameObjectToIndexMap[go];
			size_t indexOfLastElement = mSize - 1;
			mComponentArray[indexOfRemovedGameObject] = mComponentArray[indexOfLastElement];

			GameObject gameObjectOfLastElement = mIndexToGameObjectMap[indexOfLastElement];
			mGameObjectToIndexMap[gameObjectOfLastElement] = indexOfRemovedGameObject;
			mIndexToGameObjectMap[indexOfRemovedGameObject] = gameObjectOfLastElement;

			mGameObjectToIndexMap.erase(go);
			mIndexToGameObjectMap.erase(indexOfLastElement);
		}

		T& GetData(GameObject go)
		{
			assert(mGameObjectToIndexMap.find(go) != mGameObjectToIndexMap.end() && "Retrieving non-existent componnent.");

			return mComponentArray[mGameObjectToIndexMap[go]];
		}

		void GameObjectDestroyed(GameObject go) override
		{
			if (mGameObjectToIndexMap.find(go) != mGameObjectToIndexMap.end())
			{
				RemoveData(go);
			}
		}

	private:
		size_t mSize{};
		std::array<T, MAX_GAMEOBJECT> mComponentArray{};
		std::unordered_map<GameObject, size_t> mGameObjectToIndexMap{};
		std::unordered_map<size_t, GameObject> mIndexToGameObjectMap{};
};