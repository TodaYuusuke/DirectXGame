#include "WorldTransform.h"
#include "primitive/Node.h"
#include "../base/ImGuiManager.h"

using namespace LWP::Object;
using namespace LWP::Math;

WorldTransform::WorldTransform() { Initialize(); }
WorldTransform::WorldTransform(Math::Vector3 t, Math::Vector3 r, Math::Vector3 s) {
	Initialize();
	translation = t;
	rotation = r;
	scale = s;
}
WorldTransform::WorldTransform(Math::Vector3 t, Math::Vector3 r) {
	Initialize();
	translation = t;
	rotation = r;
}
WorldTransform::WorldTransform(Math::Vector3 t) {
	Initialize();
	translation = t;
}

void WorldTransform::Initialize() {
	translation = { 0.0f, 0.0f, 0.0f };
	rotation = { 0.0f, 0.0f, 0.0f };
	scale = { 1.0f, 1.0f, 1.0f };
}

Matrix4x4 WorldTransform::GetWorldMatrix() const {
	Math::Matrix4x4 result = Math::Matrix4x4::CreateAffineMatrix(scale, rotation, translation);
	// 親があれば親のワールド行列を掛ける
	if (parent_) {
		result = result * parent_->t->GetWorldMatrix();
	}
	return result;
}
Matrix4x4 WorldTransform::GetWorldMatrix(Primitive::Node* node) const {
	Math::Matrix4x4 result = node->localMatrix * Math::Matrix4x4::CreateAffineMatrix(scale, rotation, translation);
	// 親があれば親のワールド行列を掛ける
	if (parent_) {
		result = result * parent_->t->GetWorldMatrix();
	}
	return result;
}

Matrix4x4 WorldTransform::GetTranslationMatrix() const {
	Math::Matrix4x4 result = Math::Matrix4x4::CreateTranslateMatrix(translation);
	// 親があれば親のワールド行列を掛ける
	if (parent_) {
		result = result * parent_->t->GetTranslationMatrix();
	}
	return result;
}
Vector3 WorldTransform::GetWorldPosition() const {
	Math::Matrix4x4 worldMatrix = GetWorldMatrix();
	return { worldMatrix.m[3][0],worldMatrix.m[3][1],worldMatrix.m[3][2] };
}
Matrix4x4 WorldTransform::GetRotateMatrix() const {
	Math::Matrix4x4 result = Math::Matrix4x4::CreateRotateXYZMatrix(rotation);
	// 親があれば親のワールド行列を掛ける
	if (parent_) {
		result = result * parent_->t->GetRotateMatrix();
	}
	return result;
}
Matrix4x4 WorldTransform::GetScaleMatrix() const {
	Math::Matrix4x4 result = Math::Matrix4x4::CreateScaleMatrix(scale);
	// 親があれば親のワールド行列を掛ける
	if (parent_) {
		result = result * parent_->t->GetScaleMatrix();
	}
	return result;
}
Vector3 WorldTransform::GetFinalScale() const {
	Math::Matrix4x4 worldMatrix = GetScaleMatrix();
	return { worldMatrix.m[0][0],worldMatrix.m[1][1],worldMatrix.m[2][2] };
}

void WorldTransform::DebugGUI(const std::string& label) {
	if (ImGui::TreeNode(label.c_str())) {
		ImGui::DragFloat3("Translation", &translation.x, 0.01f);
		ImGui::DragFloat3("Rotation", &rotation.x, 0.01f);
		ImGui::DragFloat3("Scale", &scale.x, 0.01f);
		ImGui::TreePop();
	}
}
void WorldTransform::DebugGUI2D(const std::string& label) {
	if (ImGui::TreeNode(label.c_str())) {
		ImGui::DragFloat2("Translation", &translation.x, 1.0f);
		ImGui::DragFloat("Rotation", &rotation.z, 0.01f);
		ImGui::DragFloat2("Scale", &scale.x, 0.01f);
		ImGui::TreePop();
	}
}

// ** プロパティ変数 ** //

void WorldTransform::Parent(WorldTransform* parent) { 
	parent_ = new Utility::Observer<WorldTransform*>;
	parent_->t = parent;
}

WorldTransform WorldTransform::operator+(const WorldTransform& other) const {
	return {
		translation + other.translation,
		rotation + other.rotation,
		scale + other.scale
	};
};

WorldTransform& WorldTransform::operator+=(const WorldTransform& other) {
	return *this = *this + other;
}

//WorldTransform WorldTransform::operator/(const float& other) const {
//	return {
//		translation / other,
//		rotation / other,
//		scale / other
//	};
//}
