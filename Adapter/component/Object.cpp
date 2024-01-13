#include "Object.h"

using namespace LWP::System;
using namespace LWP;

void Object::CreateRenderTexture(LWP::Object::Camera* camera, const int width, const int height) {
	camera->CreateRenderTexture(engine->directXCommon_->GetCommandManager(), width, height);
}
