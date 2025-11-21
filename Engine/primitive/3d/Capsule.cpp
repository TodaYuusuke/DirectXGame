#include "Capsule.h"
#include "base/ImGuiManager.h"

#include "object/core/collision/collider/child/cCapsule.h"

using namespace LWP::Primitive;
using namespace LWP::Resource;
using namespace LWP::Math;
using namespace LWP::Object;

Capsule::Capsule() {
	name = "Capsule" + name + "_StartSphere"; // 名前を登録（事前に番号だけ振られてるので手前に）
	LoadFullPath("resources/system/model/standard/sphere.gltf");
	
	// 円柱と終点の初期化
	cylinder.name = name + "_Cylinder";
	//cylinder.worldTF.Parent(&worldTF);
	endSphere.name = name + "_EndSphere";
	//endSphere.worldTF.Parent(&worldTF);
}

void Capsule::ChangeFillMode() {
	RigidModel::ChangeFillMode();
	cylinder.ChangeFillMode();
	endSphere.ChangeFillMode();
}
void Capsule::SetAllMaterialColor(const Utility::Color& color) {
	materials["Material0"].color = color;
	cylinder.materials["Material0"].color = color;
	endSphere.materials["Material0"].color = color;
}
void Capsule::SetAllMaterialLighting(bool flag) {
	RigidModel::SetAllMaterialLighting(flag);
	cylinder.SetAllMaterialLighting(flag);
	endSphere.SetAllMaterialLighting(flag);
}

void Capsule::CreateFromCapsuleCol(const Math::Vector3& start, const Math::Vector3& offset, const float& radius) {
	// == 始点の球 == //
	worldTF.translation = start;
	Vector3 worldPos = worldTF.GetWorldPosition();
	Quaternion sphereQuat = Quaternion::DirectionToDirection(
		Vector3::UnitZ(),
		offset.Normalize()
	);
	worldTF.rotation = sphereQuat;	// 本体の回転も合わせる
	float s = radius / 1.0f;	// 半径からスケールを求める
	worldTF.scale = { s,s,s };	// 本体の回転も合わせる

	// == 円柱 == //
	TransformQuat& cyTF = cylinder.worldTF;
	cyTF.translation = worldPos + offset / 2.0f;
	cyTF.rotation = sphereQuat * Quaternion::CreateFromAxisAngle(Vector3::UnitX(), 1.57f);
	cyTF.scale.x = worldTF.scale.x;
	cyTF.scale.y = offset.Length() / 2.0f;
	cyTF.scale.z = worldTF.scale.y;

	// == 終点の球 == //
	TransformQuat& spTF = endSphere.worldTF;
	spTF.translation = worldPos + offset;
	spTF.rotation = sphereQuat;
	spTF.scale = worldTF.scale;
}