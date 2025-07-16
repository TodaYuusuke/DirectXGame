#include "TransformEuler.h"
#include "primitive/component/Node.h"
#include "../base/ImGuiManager.h"

using namespace LWP::Object;
using namespace LWP::Math;

TransformEuler::TransformEuler() { Init(); }
TransformEuler::TransformEuler(Math::Vector3 t, Math::Vector3 r, Math::Vector3 s) {
	Init();
	translation = t;
	rotation = r;
	scale = s;
}
TransformEuler::TransformEuler(Math::Vector3 t, Math::Vector3 r) {
	Init();
	translation = t;
	rotation = r;
}
TransformEuler::TransformEuler(Math::Vector3 t) {
	Init();
	translation = t;
}

void TransformEuler::Init() {
	translation = { 0.0f, 0.0f, 0.0f };
	rotation = { 0.0f, 0.0f, 0.0f };
	scale = { 1.0f, 1.0f, 1.0f };
}

Matrix4x4 TransformEuler::GetAffineMatrix() const {
	Math::Matrix4x4 result = Math::Matrix4x4::CreateAffineMatrix(scale, rotation, translation);
	// 親があれば親のワールド行列を掛ける
	if (parent_) {
		result = result * parent_->t->GetAffineMatrix();
	}
	return result;
}
Matrix4x4 TransformEuler::GetAffineMatrix(Primitive::Node* node) const {
	Math::Matrix4x4 result = node->GetLocalMatrix() * Math::Matrix4x4::CreateAffineMatrix(scale, rotation, translation);
	// 親があれば親のワールド行列を掛ける
	if (parent_) {
		result = result * parent_->t->GetAffineMatrix();
	}
	return result;
}

Matrix4x4 TransformEuler::GetTranslationMatrix() const {
	Math::Matrix4x4 result = Math::Matrix4x4::CreateTranslateMatrix(translation);
	// 親があれば親のワールド行列を掛ける
	if (parent_) {
		result = result * parent_->t->GetTranslationMatrix();
	}
	return result;
}
Vector3 TransformEuler::GetWorldPosition() const {
	Math::Matrix4x4 worldMatrix = GetAffineMatrix();
	return { worldMatrix.m[3][0],worldMatrix.m[3][1],worldMatrix.m[3][2] };
}
Matrix4x4 TransformEuler::GetRotateMatrix() const {
	Math::Matrix4x4 result = Math::Matrix4x4::CreateRotateXYZMatrix(rotation);
	// 親があれば親のワールド行列を掛ける
	if (parent_) {
		result = result * parent_->t->GetRotateMatrix();
	}
	return result;
}
Matrix4x4 TransformEuler::GetScaleMatrix() const {
	Math::Matrix4x4 result = Math::Matrix4x4::CreateScaleMatrix(scale);
	// 親があれば親のワールド行列を掛ける
	if (parent_) {
		result = result * parent_->t->GetScaleMatrix();
	}
	return result;
}
Vector3 TransformEuler::GetFinalScale() const {
	Math::Matrix4x4 worldMatrix = GetScaleMatrix();
	return { worldMatrix.m[0][0],worldMatrix.m[1][1],worldMatrix.m[2][2] };
}

void TransformEuler::DebugGUI(const std::string& label) {
	if (ImGui::TreeNode(label.c_str())) {
		ImGui::DragFloat3("Translation", &translation.x, 0.01f);
		ImGui::DragFloat3("Rotation", &rotation.x, 0.01f);
		ImGui::DragFloat3("Scale", &scale.x, 0.01f);
		ImGui::TreePop();
	}
}
void TransformEuler::DebugGUI2D(const std::string& label) {
	if (ImGui::TreeNode(label.c_str())) {
		ImGui::DragFloat2("Translation", &translation.x, 1.0f);
		ImGui::DragFloat("Rotation", &rotation.z, 0.01f);
		ImGui::DragFloat2("Scale", &scale.x, 0.01f);
		ImGui::TreePop();
	}
}

// ** プロパティ変数 ** //

void TransformEuler::Parent(TransformEuler* parent) { 
	parent_ = new Utility::Observer<TransformEuler*>;
	parent_->t = parent;
}

TransformEuler TransformEuler::operator+(const TransformEuler& other) const {
	return {
		translation + other.translation,
		rotation + other.rotation,
		scale + other.scale
	};
};

TransformEuler& TransformEuler::operator+=(const TransformEuler& other) {
	return *this = *this + other;
}

//WorldTransform WorldTransform::operator/(const float& other) const {
//	return {
//		translation / other,
//		rotation / other,
//		scale / other
//	};
//}
