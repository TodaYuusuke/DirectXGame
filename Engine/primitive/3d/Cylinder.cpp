#include "Cylinder.h"

using namespace LWP::Primitive;
using namespace LWP::Resource;
using namespace LWP::Math;

Cylinder::Cylinder() {
	name = "Cylinder" + name; // 名前を登録（事前に番号だけ振られてるので手前に）
	LoadFullPath("resources/system/model/standard/cylinder.gltf");
}