#pragma once

#include "../systems/system.hpp"
#include "./types.hpp"

#include <cassert>
#include <memory>
#include <unordered_map>

class SystemManager
{
	public:
		template<typename T>
		std::shared_ptr<T> RegisterSystem()
		{
			const char* typeName = typeid(T).name();
			assert(mSystems.find(typeName) ==  mSystems.end() && "Registering system more than once.");

			auto system = std::make_shared<T>();
			mSystems.insert({ typeName, system });
			return system;
		}

		template<typename T>
		void SetSignature(Signature signature)
		{
			const char* typeName = typeid(T).name();

			assert(mSystems.find(typeName) != mSystems.end() && "System used before registered");

			mSignatures.insert({ typeName, signature });
		}

		void GameObjectDestroyed(GameObject go)
		{
			for (auto const& pair : mSystems)
			{
				auto const& system = pair.second;

				system->mGameObject.erase(go);
			}
		}

		void GameObjectSignatureChanged(GameObject go, Signature gameObjectSignature)
		{
			for (auto const& pair : mSystems)
			{
				auto const& type = pair.first;
				auto const& system = pair.second;
				auto const& systemSignature = mSignatures[type];

				if ((gameObjectSignature & systemSignature) == systemSignature)
				{
					system->mGameObject.insert(go);
				}
				else
				{
					system->mGameObject.erase(go);
				}
			}
		}
	private:
		std::unordered_map<const char*, Signature> mSignatures{};

		std::unordered_map<const char*, std::shared_ptr<System>> mSystems{};
};