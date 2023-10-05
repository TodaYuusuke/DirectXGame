#include "Resource.h"

using namespace LWP::System;
using namespace LWP;

Resource::Texture* Resource::LoadTexture(const std::string& filePath) {
	engine_->resouse
	return new Resource::Texture(engine_->directXCommon_.get(), filePath);
}


Primitive::Mesh* Resource::LoadModel(const std::string& filePath) {
	Primitive::Mesh* instance = new Primitive::Mesh(engine_->primitiveManager_.get());
	instance->LoadFile(filePath);
	return instance;
}