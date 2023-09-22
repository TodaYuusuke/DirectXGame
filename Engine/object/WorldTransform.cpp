#include "WorldTransform.h"
#include "../primitive/PrimitiveManager.h"

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
	matWorld_ = new Matrix4x4();
}

void WorldTransform::CreateResource(Primitive::Manager* manager) {
	// リソースがnullでなければ戻る
	if (resource_ != nullptr) {
		return;
	}

	resource_ = manager->CreateBufferResource(sizeof(Matrix4x4));
	resource_->Map(0, nullptr, reinterpret_cast<void**>(&matWorld_));
}

Vector3 WorldTransform::GetWorldPosition() {
	Math::Matrix4x4 matWorld = MatWorld();
	return { matWorld.m[3][0],matWorld.m[3][1],matWorld.m[3][2] };
}

// ** プロパティ変数 ** //

void WorldTransform::Parent(WorldTransform* parent) { parent_ = parent; }

Matrix4x4 WorldTransform::MatWorld() {
	*matWorld_ = Math::Matrix4x4::CreateAffineMatrix(scale, rotation, translation);

	// 親があれば親のワールド行列を掛ける
	if (parent_) {
		*matWorld_ = *matWorld_ * parent_->MatWorld();
	}

	return *matWorld_;
}