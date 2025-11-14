#include "Sphere.h"
#include "base/ImGuiManager.h"
#include "object/core/collision/collider/child/cSphere.h"

using namespace LWP::Primitive;
using namespace LWP::Resource;
using namespace LWP::Math;

Sphere::Sphere() {
	name = "Sphere" + name; // 名前を登録（事前に番号だけ振られてるので手前に）
	LoadFullPath("resources/system/model/standard/sphere.gltf");
}

void Sphere::CreateFromSphereCol(const Vector3& pos, const float& rad) {
	worldTF.translation = pos;
	float s = rad / 1.0f;
	worldTF.scale = { s,s,s };
}
