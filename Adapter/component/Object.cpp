#include "Object.h"

using namespace LWP::System;
using namespace LWP;

void Object::SetPointer(Object::IObject* ptr) {
	engine->objectManager_->SetPointer(ptr);
}
void Object::DeletePointer(Object::IObject* ptr) {
	engine->objectManager_->DeletePointer(ptr);
}
void Object::CreateRenderTexture(LWP::Object::Camera* camera, const int width, const int height) {
	camera->CreateRenderTexture(engine->directXCommon_.get(), width, height);
}

void Object::Collider::SetPointer(ICollider* ptr) {
	engine->colliderManager_->SetPointer(ptr);
}
void Object::Collider::DeletePointer(ICollider* ptr) {
	engine->colliderManager_->DeletePointer(ptr);
}