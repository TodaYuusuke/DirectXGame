#include "Cube.h"
#include "base/ImGuiManager.h"
#include "object/core/collision/collider/child/cAABB.h"

using namespace LWP::Primitive;
using namespace LWP::Resource;
using namespace LWP::Math;
using namespace LWP::Utility;

Cube::Cube() {
	name = "Cube" + name; // 名前を登録（事前に番号だけ振られてるので手前に）
	LoadFullPath("resources/system/model/standard/cube.gltf");
}
void Cube::CreateFromAABB(const Math::Vector3& min, const Math::Vector3& max) {
	// 各パラメーターからScaleを調整
	
	// まずセンターを求める
	Vector3 center = (min + max) / 2.0f;
	worldTF.translation = center;
}
