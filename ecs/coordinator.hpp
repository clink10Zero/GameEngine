#pragma once

#include "./ComponentManager.hpp"
#include "./gameObjectManager.hpp"
#include "./SystemManager.hpp"
#include "./Types.hpp"
#include "../lve_device.hpp"

//for destroyed
#include "../components/graph.hpp"
#include "../components/aabb.hpp"
#include "../components/Transform.hpp"
#include "../components/mesh.hpp"
#include "../components/RigidBody.hpp"

#include <memory>
#include <vector>

#include <string>
#include <iostream>
#include <fstream>

using namespace lve;
using namespace std;

class Coordinator
{
	public:
		void Init()
		{
			mComponentManager = std::make_unique<ComponentManager>();
			mGameObjectManager = std::make_unique<GameObjectManager>();
			mSystemManager = std::make_unique<SystemManager>();
		}

		void InitFromFile(std::string path, GameObject racine)
		{
			
		}

		void SaveToFile(std::string path, GameObject racine)
		{
			ofstream flux(path.c_str());

			SaveGameObject(racine, flux);

			Graph g = this->GetCompenent<Graph>(racine);
			for (auto& go : g.children)
			{
				SaveGameObject(go, flux);
			}
			flux.close();
		}

		void SaveGameObject(GameObject go, ofstream &flux)
		{
			flux << "transform" << endl;
			Transform t = this->GetCompenent<Transform>(go);
			WriteVec(t.translation, flux);
			WriteVec(t.rotation, flux);
			WriteVec(t.scale, flux);

			flux << "graph" << endl;
			Graph g = this->GetCompenent<Graph>(go);
			flux << g.parent << endl;
			size_t sizeChildren = g.children.size();
			for (size_t i = 0; i < sizeChildren; i++)
			{
				flux << g.children[i] << endl;
			}

			if (this->HaveComponent<Mesh>(go))
			{
				flux << "mesh" << endl;
				Mesh m = this->GetCompenent<Mesh>(go);
				flux << m.path << endl;
				flux << m.lod << endl;
			}

			if (this->HaveComponent<AABB>(go))
			{
				flux << "aabb" << endl;
			}

			if (this->HaveComponent<RigidBody>(go))
			{
				flux << "rigidBody" << endl;
				RigidBody r = this->GetCompenent<RigidBody>(go);
				WriteVec(r.acceleration, flux);
				WriteVec(r.velocity, flux);
				WriteVec(r.forceGravity, flux);
			}
		}

		static void WriteVec(glm::vec3 vec, ofstream& flux)
		{
			flux << vec.x << " " << vec.y << " " << vec.z << endl;
		}

		//GameObject methode
		GameObject CreateGameObject()
		{
			return mGameObjectManager->CreateGameObject();
		}

		void DestroyedGameObject(GameObject go)
		{
			/*
			if (this->HaveComponent<Graph>(go))
			{
				Graph g = this->GetCompenent<Graph>(go);
				if (this->HaveComponent<Graph>(g.parent))
				{
					Graph gp = this->GetCompenent<Graph>(g.parent);

					gp.children.erase(std::remove(gp.children.begin(), gp.children.end(), 8), gp.children.end());

					for (auto& obj : g.children) {
						DestroyedGameObject(obj);
					}
				}
			}
			*/
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

			auto signature = mGameObjectManager->GetSignature(go);
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
		std::shared_ptr<ComponentManager> mComponentManager;
		std::shared_ptr<GameObjectManager> mGameObjectManager;
		std::shared_ptr<SystemManager> mSystemManager;
};