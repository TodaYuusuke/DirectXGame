#include "WorldTransform.h"
#include "../base/ImGuiManager.h"

using namespace LWP::Primitive;
using namespace LWP::Object;
using namespace LWP::Math;

WorldTransform::WorldTransform() { Initialize(); }
WorldTransform::WorldTransform(Math::Vector3 s, Math::Vector3 r, Math::Vector3 t) {
	Initialize();
	scale = s;
	rotation = r;
	translation = t;
}
WorldTransform::WorldTransform(Math::Vector3 r, Math::Vector3 t) {
	Initialize();
	rotation = r;
	translation = t;
}
WorldTransform::WorldTransform(Math::Vector3 t) {
	Initialize();
	translation = t;
}

void WorldTransform::Initialize() {
	scale = { 1.0f, 1.0f, 1.0f };
	rotation = { 0.0f, 0.0f, 0.0f };
	translation = { 0.0f, 0.0f, 0.0f };
}

Matrix4x4 WorldTransform::GetWorldMatrix() const {
	Math::Matrix4x4 result = Math::Matrix4x4::CreateAffineMatrix(scale, rotation, translation);

	// 親があれば親のワールド行列を掛ける
	if (parent_) {
		result = result * parent_->GetWorldMatrix();
	}
	return result;
}
Vector3 WorldTransform::GetWorldPosition() const {
	Math::Matrix4x4 worldMatrix = GetWorldMatrix();
	return { worldMatrix.m[3][0],worldMatrix.m[3][1],worldMatrix.m[3][2] };
}

void WorldTransform::DebugGUI(const std::string& label) {
	if (ImGui::TreeNode(label.c_str())) {
		ImGui::DragFloat3("Translation", &translation.x, 0.01f);
		ImGui::DragFloat3("Rotation", &rotation.x, 0.01f);
		ImGui::DragFloat3("Scale", &scale.x, 0.01f);
		ImGui::TreePop();
	}
}

// ** プロパティ変数 ** //

void WorldTransform::Parent(WorldTransform* parent) { parent_ = parent; }