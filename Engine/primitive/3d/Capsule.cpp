#include "Capsule.h"
#include "base/ImGuiManager.h"

#include "object/core/collision/collider/child/cCapsule.h"

using namespace LWP::Primitive;
using namespace LWP::Resource;
using namespace LWP::Math;

Capsule::Capsule() {
	name = "Capsule" + name; // 名前を登録（事前に番号だけ振られてるので手前に）
	LoadFullPath("resources/system/model/standard/cylinder.gltf");
	startSphere.worldTF.Parent(&worldTF);
	endSphere.worldTF.Parent(&worldTF);
}

void Capsule::Update() {
	if (!isActive) { return; }


}
void Capsule::DebugGUI() {
	ImGui::DragFloat3("localEndOffset", &localOffset.x, 0.01f);
	RigidModel::DebugGUI();
}

void Capsule::CreateFromCapsuleCol(const Vector3& s, const Vector3& e, const float& r) {
	worldTF.translation = s;
	localOffset = e;
	r;
}