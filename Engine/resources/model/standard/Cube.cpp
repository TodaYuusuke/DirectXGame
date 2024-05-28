#include "Cube.h"

#include "base/directX/RendererManager.h"
#include "resources/ResourceManager.h"
#include "component/Resource.h"
#include "component/System.h"

using namespace LWP;
using namespace LWP::Base;
using namespace LWP::Math;
using namespace LWP::Resource;


Cube::Cube() : RigidModel::RigidModel() {
	LoadFullPath("resources/system/model/standard/cube.gltf");
}
Cube::Cube(const Cube& other) : Cube() {
	worldTF = other.worldTF;
	enableLighting = other.enableLighting;
	isActive = other.isActive;
}


void Cube::DebugGUI() {
	worldTF.DebugGUI();
	if (ImGui::TreeNode("Materials")) {
		for (int i = 0; i < materials.size(); i++) {
			materials[i].DebugGUI(std::to_string(i));
		}
		ImGui::TreePop();
	}
	ImGui::Checkbox("enableLighting", &enableLighting);
	ImGui::Checkbox("isWireFrame", &isWireFrame);
	ImGui::Checkbox("isActive", &isActive);
}