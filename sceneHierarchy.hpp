#pragma once

#include "ecs/coordinator.hpp"
#include "ecs/types.hpp"

#include "components/Transform.hpp"
#include "components/graph.hpp"
#include "components/RigidBody.hpp"
#include "components/mesh.hpp"
#include "components/aabb.hpp"
#include "components/terrain.hpp"
#include "components/Camera.hpp"
#include "components/MotionControl.hpp"
#include "components/interTerrain.h"

namespace lve {
    class SceneHierarchyPanel {
    public:
        SceneHierarchyPanel() = default;

        void OnImGuiRender(GameObject racine);

        GameObject GetSelectedEntity() const { return selected; }
    private:
        
        void DrawEntityNode(GameObject go);
        void DrawComponents(GameObject go);
        template<typename T, typename UIFunction>
        void DrawComponent(const std::string& name, GameObject go, bool removable, UIFunction uiFunction);
        GameObject racine;
        GameObject selected = -1;
    };
}