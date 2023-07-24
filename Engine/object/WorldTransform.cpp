#include "WorldTransform.h"

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

Vector3 WorldTransform::GetWorldPosition() {
	Math::Matrix4x4 matWorld = GetMatWorld();
	return { matWorld.m[3][0],matWorld.m[3][1],matWorld.m[3][2] };
}

// ** プロパティ変数 ** //

void WorldTransform::SetParent(const WorldTransform* parent) { parent_ = parent; }

Matrix4x4 WorldTransform::GetMatWorld() const {
	Math::Matrix4x4 result = Math::Matrix4x4::CreateAffineMatrix(scale, rotation, translation);

	// 親があれば親のワールド行列を掛ける
	if (parent_) {
		result = result * parent_->GetMatWorld();
	}

	return result;
}