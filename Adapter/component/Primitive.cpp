#include "Primitive.h"

using namespace LWP::System;
using namespace LWP;

template <IsIPrimitive TPrimitive>
static TPrimitive* Primitive::CreateInstance() {
	TPrimitive* instance = new TPrimitive(engine_->primitiveManager_.get());
	instance->CreateVertices();
	return instance;
}

void Primitive::SetMainCameraMatrix(Object::Camera* camera) {
	engine_->primitiveManager_->SetViewProjection2D(camera->GetViewProjectionMatrix2D());
	engine_->primitiveManager_->SetViewProjection3D(camera->GetViewProjectionMatrix3D());
}
