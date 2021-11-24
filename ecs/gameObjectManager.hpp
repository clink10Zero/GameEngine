#pragma once

#include "./types.hpp"

#include <queue>
#include <array>
#include <cassert>

class GameObjectManager
{
	public:
		GameObjectManager()
		{
			for (GameObject go = 0; go < MAX_GAMEOBJECT; go++) {
				mAvailableGameObjects.push(go);
			}
		}
		
		GameObject CreateGameObject()
		{
			assert(mLivingGameObjectCount < MAX_GAMEOBJECT && "Too many gameobject in existance.");

			GameObject id = mAvailableGameObjects.front();
			mAvailableGameObjects.pop();
			++mLivingGameObjectCount;
			return id;
		}

		void DestroyedGameObject(GameObject go)
		{
			assert(go < MAX_GAMEOBJECT && "gameObject out of range.");
			mSignatures[go].reset();
			mAvailableGameObjects.push(go);
			--mLivingGameObjectCount;
		}

		void SetSignature(GameObject go, Signature signature)
		{
			assert(go < MAX_GAMEOBJECT && "GameObject out of range.");
			mSignatures[go] = signature;
		}

		Signature GetSignature(GameObject go) {
			assert(go < MAX_GAMEOBJECT && "Entity out of range.");
			return mSignatures[go];
		}

	private:
		std::queue<GameObject> mAvailableGameObjects{};
		std::array<Signature, MAX_GAMEOBJECT> mSignatures{};

		uint32_t mLivingGameObjectCount{};
};