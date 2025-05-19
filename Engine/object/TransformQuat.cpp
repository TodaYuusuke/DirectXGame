#include "TransformQuat.h"
#include "primitive/model/Node.h"
#include "../base/ImGuiManager.h"

using namespace LWP::Object;
using namespace LWP::Math;

TransformQuat::TransformQuat() { Init(); }
TransformQuat::~TransformQuat() {}
TransformQuat::TransformQuat(Math::Vector3 t, Math::Quaternion r, Math::Vector3 s) {
	Init();
	translation = t;
	rotation = r;
	scale = s;
}
TransformQuat::TransformQuat(Math::Vector3 t, Math::Quaternion r) {
	Init();
	translation = t;
	rotation = r;
}
TransformQuat::TransformQuat(Math::Vector3 t) {
	Init();
	translation = t;
}


void TransformQuat::Init() {
	translation = { 0.0f, 0.0f, 0.0f };
	rotation.Init();
	scale = { 1.0f, 1.0f, 1.0f };
}

Matrix4x4 TransformQuat::GetAffineMatrix() const {
	Math::Matrix4x4 result = Math::Matrix4x4::CreateAffineMatrix(scale, rotation, translation);
	// 親があれば親のワールド行列を掛ける
	if (CheckParent()) {
		result = result * parent_->GetAffineMatrix();
	}
	return result;
}
Matrix4x4 TransformQuat::GetAffineMatrix(Primitive::Node* node) const {
	Math::Matrix4x4 result = node->GetLocalMatrix() * Math::Matrix4x4::CreateAffineMatrix(scale, rotation, translation);
	// 親があれば親のワールド行列を掛ける
	if (CheckParent()) {
		result = result * parent_->GetAffineMatrix();
	}
	return result;
}

Matrix4x4 TransformQuat::GetTranslationMatrix() const {
	Math::Matrix4x4 result = Math::Matrix4x4::CreateTranslateMatrix(translation);
	// 親があれば親のワールド行列を掛ける
	if (CheckParent()) {
		result = result * parent_->GetTranslationMatrix();
	}
	return result;
}
Vector3 TransformQuat::GetWorldPosition() const {
	Math::Matrix4x4 worldMatrix = GetAffineMatrix();
	return { worldMatrix.m[3][0],worldMatrix.m[3][1],worldMatrix.m[3][2] };
}
Matrix4x4 TransformQuat::GetRotateMatrix() const {
	Math::Matrix4x4 result = Math::Matrix4x4::CreateRotateXYZMatrix(rotation);
	// 親があれば親のワールド行列を掛ける
	if (CheckParent()) {
		result = result * parent_->GetRotateMatrix();
	}
	return result;
}
Matrix4x4 TransformQuat::GetScaleMatrix() const {
	Math::Matrix4x4 result = Math::Matrix4x4::CreateScaleMatrix(scale);
	// 親があれば親のワールド行列を掛ける
	if (CheckParent()) {
		result = result * parent_->GetScaleMatrix();
	}
	return result;
}
Vector3 TransformQuat::GetFinalScale() const {
	Math::Matrix4x4 worldMatrix = GetScaleMatrix();
	return { worldMatrix.m[0][0],worldMatrix.m[1][1],worldMatrix.m[2][2] };
}

void TransformQuat::DebugGUI(const std::string& label) {
	if (ImGui::TreeNode(label.c_str())) {
		ImGui::DragFloat3("Translation", &translation.x, 0.01f);
		Vector3 rot = { 0.0f,0.0f,0.0f };
		ImGui::DragFloat3("Rotation", &rot.x, 0.01f);
		rotation = Quaternion::ConvertEuler(rot) * rotation;
		ImGui::DragFloat4("Quaternion", &rotation.x, 0.01f);
		ImGui::Text("Norm ... %f", rotation.Length());
		if (ImGui::Button("Init Quaternion")) { rotation.Init(); }
		ImGui::DragFloat3("Scale", &scale.x, 0.01f);
		// ペアレント先を表示
		if (CheckParent()) { parent_->DebugGUI("Parent"); }
		else { ImGui::Text("Not Parent"); }
		ImGui::TreePop();
	}
}
void TransformQuat::DebugGUI2D(const std::string& label) {
	if (ImGui::TreeNode(label.c_str())) {
		ImGui::DragFloat2("Translation", &translation.x, 1.0f);
		float rot = 0.0f;
		ImGui::DragFloat("Rotation", &rot, 0.01f);
		rotation = Quaternion::ConvertEuler({ 0.0f,0.0f,rot }) * rotation;
		ImGui::DragFloat4("Quaternion", &rotation.x, 0.01f);
		ImGui::Text("Norm ... %f", rotation.Length());
		if (ImGui::Button("Init Quaternion")) { rotation.Init(); }
		ImGui::DragFloat2("Scale", &scale.x, 0.01f);
		ImGui::TreePop();
	}
}

bool TransformQuat::CheckParent() const {
	return parent_;
}

// ** プロパティ変数 ** //

//void TransformQuat::Parent(TransformQuat* parent) {
//	if (!parent_) { parent_ = new Utility::Observer<TransformQuat*>; }
//
//	if (parent_->t) { parent_->t = nullptr; }
//	parent_->t = parent;
//}

TransformQuat TransformQuat::operator+(const TransformQuat& other) const {
	return {
		translation + other.translation,
		rotation + other.rotation,
		scale + other.scale
	};
};

TransformQuat& TransformQuat::operator+=(const TransformQuat& other) {
	return *this = *this + other;
}
