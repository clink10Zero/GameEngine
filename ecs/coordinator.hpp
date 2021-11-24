#pragma once

#include "./ComponentManager.hpp"
#include "./gameObjectManager.hpp"
#include "./SystemManager.hpp"
#include "./Types.hpp"

#include <memory>

class Coordinator
{
	public:
		void Init()
		{
			mComponentManager = std::make_unique<ComponentManager>();
			mGameObjectManager = std::make_unique<GameObjectManager>();
			mSystemManager = std::make_unique<SystemManager>();
		}

		//GameObject methode
		GameObject CreateGameObject()
		{
			return mGameObjectManager->CreateGameObject();
		}

		void DestroyedGameObject(GameObject go)
		{
			mGameObjectManager->DestroyedGameObject(go);
			mComponentManager->GameObjectDestroyed(go);
			mSystemManager->GameObjectDestroyed(go);
		}

		//Component methods
		template<typename T>
		void RegisterComponent()
		{
			mComponentManager->RegisterComponnent<T>();
		}

		template<typename T>
		void AddComponent(GameObject go, T component)
		{
			mComponentManager->AddComponent<T>(go, component);

			auto signature = mGameObjectManager->GetSignature(go);
			signature.set(mComponentManager->GetComponentType<T>(), true);
			mGameObjectManager->SetSignature(go, signature);
			
			mSystemManager->GameObjectSignatureChanged(go, signature);
		}

		template<typename T>
		void RemoveComponent(GameObject go) {
			mComponentManager->RemoveComponent<T>(go);

			auto signature = mGameObjectManager<T>(go);
			signature.set(mComponentManager->GetComponentType<T>(), false);
			mGameObjectManager->SetSignature(go, signature);

			mSystemManager->GameObjectSignatureChanged(go, signature);
		}

		template<typename T>
		T& GetCompenent(GameObject go)
		{
			return mComponentManager->GetComponent<T>(go);
		}


		template<typename T>
		ComponentType GetComponentType()
		{
			return mComponentManager->GetComponentType<T>();
		}

		template<typename T>
		bool HaveComponent(GameObject go)
		{
			return mComponentManager->HaveComponent<T>(go);
		}


		//system methods
		template<typename T>
		std::shared_ptr<T> RegisterSystem()
		{
			return mSystemManager->RegisterSystem<T>();
		}

		template<typename T>
		void SetSystemSignature(Signature signature)
		{
			mSystemManager->SetSignature<T>(signature);
		}

	private:
		std::unique_ptr<ComponentManager> mComponentManager;
		std::unique_ptr<GameObjectManager> mGameObjectManager;
		std::unique_ptr<SystemManager> mSystemManager;
};