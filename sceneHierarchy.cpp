#include "sceneHierarchy.hpp"

#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include "print.hpp"

#include <string.h>

using namespace printGUI;

extern Coordinator gCoordinator;

extern Print affichage;

void SceneHierarchyPanel::OnImGuiRender(GameObject racine)
{
	if (affichage.hierarchy)
	{
		ImGui::Begin("Scene Hierarchy");
		Graph g = gCoordinator.GetCompenent<Graph>(racine);

		for (auto& go : g.children)
		{
			DrawEntityNode(go);
		}
		ImGui::End();
	}
	if (affichage.component) {
		if (selected)
		{
			ImGui::Begin("component");
			DrawComponents(selected);
			ImGui::End();
		}
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
		Graph g = gCoordinator.GetCompenent<Graph>(go);

		for (auto& cgo : g.children)
		{
			DrawEntityNode(cgo);
		}
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
			glm::vec3 rotation = glm::degrees(component.rotation);
			DrawVec3Control("Rotation", rotation, 0.0f, 100.f);
			component.rotation = glm::radians(rotation);
			DrawVec3Control("Scale", component.scale, 1.0f, 100.f);
		});

	//TODO input pour modification direct
	DrawComponent<Graph>("Graph", go, false, [](auto& component)
		{
			ImGui::Text(("Parent : " + std::to_string(component.parent)).c_str());
			for (int i = 0; i < component.children.size(); i++)
			{
				ImGui::Text((std::to_string(i) + " - gameobject " + std::to_string(component.children[i])).c_str());
			}
		});

	DrawComponent<AABB>("AABB", go, true, [](auto& component)
		{
			DrawVec3Control("Min", component.min, -100.f, 100.f);
			DrawVec3Control("Max", component.max, -100.f, 100.f);
		});

	DrawComponent<RigidBody>("RigideBody", go, true, [](auto& component)
		{
			DrawVec3Control("Velocity", component.velocity, -100.f, 100.f);
			DrawVec3Control("Acceleration", component.acceleration, -100.f, 100.f);
			DrawVec3Control("Gravity Force", component.forceGravity, -100.f, 100.f);
		});

	DrawComponent<Mesh>("Mesh", go, true, [](auto& component)
		{
			std::string old_path = component.path;
			int old_lod = component.lod;
			ImGui::Text(component.path.c_str());
			ImGui::SliderInt("LOD", &component.lod, 0, 3);
		});

	DrawComponent<Terrain>("Terrain", go, true, [](auto& component)
		{
			ImGui::DragInt("Seed", &component.seed, 1, 0, 10000000000000000000);
			
			ImGui::SliderInt("Xsize", &component.Xsize, 0, INT16_MAX);
			ImGui::SliderInt("Ysize", &component.Ysize, 0, INT16_MAX);
			ImGui::SliderInt("Zsize", &component.Zsize, 0, INT16_MAX);

			ImGui::SliderInt("Octave", &component.octave, 0, 8);
			ImGui::SliderFloat("Bias", &component.scalingBiais, 0.2f, 2.f);

			ImGui::DragFloat("Seuil", &component.seuil, 0.2f, 0.f, 1.f);

			ImGui::SliderInt("modificateur", &component.modificateur, 0, 32);

		});
	DrawComponent<Chunk>("Chunk", go, true, [](auto& component)
		{

			if (ImGui::Button("data", ImVec2{ 50, 20 }))
				affichage.dataChunk = !affichage.dataChunk;

			if (affichage.dataChunk)
			{
				ImGui::Begin("Data chunk");
				if (ImGui::Button("Height", ImVec2{ 100, 20 }))
				{
					affichage.block = false;
					affichage.height = true;
				}
				
				ImGui::SameLine();
				if (ImGui::Button("Block", ImVec2{ 100, 20 }))
				{
					affichage.block = true;
					affichage.height = false;
				}

				ImGui::SameLine();
				if (ImGui::SmallButton("x"))
					affichage.dataChunk = false;

				ImGui::Separator();
				if (affichage.block)
				{
					ImGui::SliderInt("niveau", &component.niveau, 0, component.Ysize - 1);
					ImGui::Separator();
				}
				ImGui::BeginTable("data", component.Ysize);

				for (int x = 0; x < component.Xsize; x++)
				{
					for (int z = 0; z < component.Zsize; z++)
					{
						ImGui::TableNextColumn();
						if (affichage.block)
						{
							if (component.data[x][component.niveau][z].sol)
								ImGui::Text("t");
							else
								ImGui::Text("f");
						}

						if (affichage.height)
						{
							ImGui::Text(std::to_string(component.height[x * component.Xsize + z]).c_str());
						}
					}
					ImGui::TableNextRow();
				}
				ImGui::EndTable();
				ImGui::End();
			}
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
		{
			if (gCoordinator.HaveComponent<Mesh>(go) && strcmp(typeid(T).name(),"Mesh")) {
				gCoordinator.RemoveComponent<AABB>(go);
			}
			gCoordinator.RemoveComponent<T>(go);
		}
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