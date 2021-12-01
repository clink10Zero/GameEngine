#include "sceneHierarchy.hpp"

#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"

#include <string.h>

extern Coordinator gCoordinator;

void SceneHierarchyPanel::OnImGuiRender(GameObject racine)
{
	ImGui::Begin("Scene Hierarchy");
	Graph g = gCoordinator.GetCompenent<Graph>(racine);

	for (auto& go : g.children)
	{
		OnImGuiGameObject(go);
	}
	ImGui::End();

	if (selected)
	{
		ImGui::Begin("component");
		DrawComponents(selected);
		ImGui::End();
	}
}
void SceneHierarchyPanel::OnImGuiGameObject(GameObject go)
{
	DrawEntityNode(go);
	Graph g = gCoordinator.GetCompenent<Graph>(go);

	for (auto& cgo : g.children)
	{
		OnImGuiGameObject(cgo);
	}
}

void SceneHierarchyPanel::DrawEntityNode(GameObject go)
{
	ImGuiTreeNodeFlags flags = ((selected == go) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
	flags |= ImGuiTreeNodeFlags_SpanAvailWidth;
	
	bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)go, flags, ("gameobject" + std::to_string(go)).c_str());
	if (ImGui::IsItemClicked()) {
		selected = go;
	}

	bool gameObjectDeleted = false;
	if (ImGui::BeginPopupContextItem()) {
		if (ImGui::MenuItem("Delete Entity"))
			gameObjectDeleted = true;

		ImGui::EndPopup();
	}

	if (opened) {
		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
		bool opened = ImGui::TreeNodeEx((void*)9817239, flags, std::to_string(go).c_str());
		if (opened)
			ImGui::TreePop();
		ImGui::TreePop();
	}

	if (gameObjectDeleted)
	{
		gCoordinator.DestroyedGameObject(go);
		if (selected == go)
		{
			selected = {};
		}
	}
}

void SceneHierarchyPanel::DrawComponents(GameObject go)
{
	ImGui::Text(("gameobject" + std::to_string(go)).c_str());
	ImGui::SameLine();
	ImGui::PushItemWidth(-1);
	
	if (ImGui::Button("Add Component"))
		ImGui::OpenPopup("AddComponent");

	if (ImGui::BeginPopup("AddComponent")) {
		//TODO addfunction
		ImGui::EndPopup();
	}

	ImGui::PopItemWidth();

	DrawComponent<Transform>("Transform", go, false, [](auto& component)
	{
		DrawVec3Control("Translation", component.translation, 0.0f, 100.f);
		glm::vec3 rotation = component.rotation;
		DrawVec3Control("Rotation", rotation, 0.0f, 100.f);
		component.rotation = rotation;
		DrawVec3Control("Scale", component.scale, 1.0f, 100.f);
	});

	DrawComponent<Graph>("Graph", go, false, [](auto& component)
	{
		ImGui::Text(("parent : " + std::to_string(component.parent)).c_str());
		for (int i = 0; i < component.children.size(); i++)
		{
			ImGui::Text((std::to_string(i) + " - gameobject " + std::to_string(component.children[i])).c_str());
		}
	});

	DrawComponent<AABB>("AABB", go, true, [](auto& component)
	{
		DrawVec3Control("min", component.min, -100.f, 100.f);
		DrawVec3Control("max", component.max, -100.f, 100.f);
	});

}

template<typename T, typename UIFunction>
void SceneHierarchyPanel::DrawComponent(const std::string& name, GameObject go, bool removable, UIFunction uiFunction)
{
	const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding;
	if (gCoordinator.HaveComponent<T>(go))
	{
		auto& component = gCoordinator.GetCompenent<T>(go);
		ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4 });
		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImGui::Separator();
		bool open = ImGui::TreeNodeEx((void*)typeid(T).hash_code(), treeNodeFlags, name.c_str());
		ImGui::PopStyleVar();

		
		if (removable) {
			ImGui::SameLine(contentRegionAvailable.x - lineHeight * 0.5f);

			if (ImGui::Button("+", ImVec2{ lineHeight, lineHeight }))
			{
				ImGui::OpenPopup("ComponentSettings");
			}
		}
		bool removeComponent = false;
		if (ImGui::BeginPopup("ComponentSettings")) {
			if (ImGui::MenuItem("Remove component"))
				removeComponent = true;

			ImGui::EndPopup();
		}

		if (open) {
			uiFunction(component);
			ImGui::TreePop();
		}

		if (removeComponent)
			gCoordinator.RemoveComponent<T>(go);
	}
}

static void DrawVec3Control(const std::string& label, glm::vec3& values, float resetValue = 0.0f, float columnWidth = 100.0f) {
	ImGuiIO& io = ImGui::GetIO();
	auto boldFont = io.Fonts->Fonts[0];

	ImGui::PushID(label.c_str());

	ImGui::Columns(2);
	ImGui::SetColumnWidth(0, columnWidth);
	ImGui::Text(label.c_str());
	ImGui::NextColumn();

	ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

	float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
	ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
	ImGui::PushFont(boldFont);
	if (ImGui::Button("X", buttonSize))
		values.x = resetValue;
	ImGui::PopFont();
	ImGui::PopStyleColor(3);

	ImGui::SameLine();
	ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
	ImGui::PopItemWidth();
	ImGui::SameLine();

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
	ImGui::PushFont(boldFont);
	if (ImGui::Button("Y", buttonSize))
		values.y = resetValue;
	ImGui::PopFont();
	ImGui::PopStyleColor(3);

	ImGui::SameLine();
	ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
	ImGui::PopItemWidth();
	ImGui::SameLine();

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
	ImGui::PushFont(boldFont);
	if (ImGui::Button("Z", buttonSize))
		values.z = resetValue;
	ImGui::PopFont();
	ImGui::PopStyleColor(3);

	ImGui::SameLine();
	ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f");
	ImGui::PopItemWidth();

	ImGui::PopStyleVar();

	ImGui::Columns(1);

	ImGui::PopID();
}