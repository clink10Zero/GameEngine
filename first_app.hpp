#pragma once

#include "lve_device.hpp"
#include "lve_swap_chain.hpp"
#include "lve_window.hpp"
#include "lve_game_object.hpp"
#include "lve_renderer.hpp"
#include "lve_descriptors.hpp"

#include "./ecs/coordinator.hpp"
#include "sceneHierarchy.hpp"

// std
#include <memory>
#include <vector>

namespace lve {
	class FirstApp {
	 public:
		static constexpr int WIDTH = 1280;
		static constexpr int HEIGHT = 720;

		FirstApp();
		~FirstApp();

		FirstApp(const FirstApp &) = delete;
		FirstApp &operator=(const FirstApp &) = delete;

		void run();

	private:
		void loadGameObject();
		void loadCoordinateur();
		GameObject racine;

		LveWindow lveWindow{WIDTH, HEIGHT, "PouleEngine 0.2v (Toujours pas Minecraft! XOXO)"};
		LveDevice lveDevice{lveWindow};
		LveRenderer lveRenderer{ lveWindow, lveDevice };
		std::unique_ptr<LveDescriptorPool> globalPool{};

		//info pour le Play/Pause button
		bool editor = true;
		std::string playLabel = "Play";

		Coordinator gCoordinatorSaveEditor;

		SceneHierarchyPanel hierarchy{};
	};
}