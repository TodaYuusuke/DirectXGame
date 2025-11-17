#include "Capsule.h"
#include "base/ImGuiManager.h"

#include "object/core/collision/collider/child/cCapsule.h"

using namespace LWP::Primitive;
using namespace LWP::Resource;
using namespace LWP::Math;
using namespace LWP::Object;

Capsule::Capsule() {
	name = "Capsule" + name; // 名前を登録（事前に番号だけ振られてるので手前に）
	LoadFullPath("resources/system/model/standard/sphere.gltf");
	
	// 始点と終点の初期化
	cylinder.name = name + "_Cylinder";
	//cylinder.worldTF.Parent(&worldTF);
	endSphere.name = name + "_EndSphere";
	//endSphere.worldTF.Parent(&worldTF);
}

void Capsule::Update() {
	if (!isActive) { return; }

	// 傾き
	Quaternion sphereQuat = Quaternion::DirectionToDirection(
		Vector3::UnitZ(),
		localOffset.Normalize()
	);
	worldTF.rotation = sphereQuat;	// 本体の回転も合わせる

	// 円柱調整
	TransformQuat& cyTF = cylinder.worldTF;
	cyTF.translation = localOffset / 2.0f;
	cyTF.rotation = sphereQuat * Quaternion::CreateFromAxisAngle(Vector3::UnitX(), 1.57f);
	cyTF.scale.x = worldTF.scale.x;
	cyTF.scale.y = localOffset.Length() / 2.0f;
	cyTF.scale.z = worldTF.scale.y;

	// 終点の座標調整
	TransformQuat& spTF = endSphere.worldTF;
	spTF.translation = localOffset;
	spTF.rotation = sphereQuat;
	spTF.scale.x = worldTF.scale.x;
	spTF.scale.y = worldTF.scale.y;
}
void Capsule::DebugGUI() {
	ImGui::DragFloat3("localEndOffset", &localOffset.x, 0.01f);
	RigidModel::DebugGUI();
}
void Capsule::ChangeFillMode() {
	RigidModel::ChangeFillMode();
	cylinder.ChangeFillMode();
	endSphere.ChangeFillMode();
}
void Capsule::SetAllMaterialLighting(bool flag) {
	RigidModel::SetAllMaterialLighting(flag);
	cylinder.SetAllMaterialLighting(flag);
	endSphere.SetAllMaterialLighting(flag);
}

void Capsule::CreateFromCapsuleCol(const Vector3& s, const Vector3& e, const float& r) {
	worldTF.translation = s;
	localOffset = e;
	r;
}