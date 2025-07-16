#include "TransformQuat.h"
#include "primitive/component/Node.h"
#include "resources/model/SkinningModel.h"
#include "../base/ImGuiManager.h"

using namespace LWP::Object;
using namespace LWP::Math;


void TransformQuat::ClearParent() {
	parent_.type = ParentTargetType::None;
	parent_.worldTF = nullptr;
	parent_.localTF = nullptr;
}
void TransformQuat::Parent(TransformQuat* parent) {
	parent_.type = ParentTargetType::Transform;
	parent_.worldTF = parent;
	parent_.localTF = nullptr;
}
// 親関係を登録（スキンモデル）
void TransformQuat::Parent(Resource::SkinningModel* model, const std::string& jointName) {
	parent_.type = ParentTargetType::Joint;
	parent_.worldTF = &model->worldTF;
	parent_.localTF = &model->GetJoint(jointName)->localTF;
}

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
	switch (parent_.type) {
		case LWP::Object::TransformQuat::ParentTargetType::Transform:
			result = result * parent_.worldTF->GetAffineMatrix();
			break;
		case LWP::Object::TransformQuat::ParentTargetType::Joint:
			result = parent_.localTF->GetAffineMatrix() * result * parent_.worldTF->GetAffineMatrix();
			break;
	}
	return result;
}
Matrix4x4 TransformQuat::GetAffineMatrix(Primitive::Node* node) const {
	Math::Matrix4x4 result = node->GetLocalMatrix() * Math::Matrix4x4::CreateAffineMatrix(scale, rotation, translation);
	// 親があれば親のワールド行列を掛ける
	switch (parent_.type) {
		case LWP::Object::TransformQuat::ParentTargetType::Transform:
			result = result * parent_.worldTF->GetAffineMatrix();
			break;
		case LWP::Object::TransformQuat::ParentTargetType::Joint:
			result = parent_.localTF->GetAffineMatrix() * result * parent_.worldTF->GetAffineMatrix();
			break;
	}
	return result;
}

Matrix4x4 TransformQuat::GetTranslationMatrix() const {
	Math::Matrix4x4 result = Math::Matrix4x4::CreateTranslateMatrix(translation);
	// 親があれば親のワールド行列を掛ける
	switch (parent_.type) {
		case LWP::Object::TransformQuat::ParentTargetType::Transform:
			result = result * parent_.worldTF->GetTranslationMatrix();
			break;
		case LWP::Object::TransformQuat::ParentTargetType::Joint:
			result = parent_.localTF->GetTranslationMatrix() * result * parent_.worldTF->GetTranslationMatrix();
			break;
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
	switch (parent_.type) {
		case LWP::Object::TransformQuat::ParentTargetType::Transform:
			result = result * parent_.worldTF->GetRotateMatrix();
			break;
		case LWP::Object::TransformQuat::ParentTargetType::Joint:
			result = parent_.localTF->GetRotateMatrix() * result * parent_.worldTF->GetRotateMatrix();
			break;
	}
	return result;
}
Matrix4x4 TransformQuat::GetScaleMatrix() const {
	Math::Matrix4x4 result = Math::Matrix4x4::CreateScaleMatrix(scale);
	// 親があれば親のワールド行列を掛ける
	switch (parent_.type) {
		case LWP::Object::TransformQuat::ParentTargetType::Transform:
			result = result * parent_.worldTF->GetScaleMatrix();
			break;
		case LWP::Object::TransformQuat::ParentTargetType::Joint:
			result = parent_.localTF->GetScaleMatrix() * result * parent_.worldTF->GetScaleMatrix();
			break;
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
		switch (parent_.type) {
			case LWP::Object::TransformQuat::ParentTargetType::None:
				ImGui::Text("No Parent");
				break;
			case LWP::Object::TransformQuat::ParentTargetType::Transform:
				parent_.worldTF->DebugGUI("Parent");
				break;
			case LWP::Object::TransformQuat::ParentTargetType::Joint:
				parent_.worldTF->DebugGUI("Parent");
				parent_.localTF->DebugGUI("Joint");
				break;
		}
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
