#include "CameraManager.h"

using namespace LWP;
using namespace LWP::Math;
using namespace LWP::Input;
using namespace LWP::Primitive;
using namespace LWP::Object;

// 初期化
void CameraManager::Init(LWP::Resource::LevelData* level, LWP::Object::Camera* camera) {
	level_ = level;
	camera_ = camera;
}
