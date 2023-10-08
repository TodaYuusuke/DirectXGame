#include "Resource.h"

using namespace LWP::System;
using namespace LWP;

Resource::Texture* Resource::LoadTexture(const std::string& filePath) {
	return engine->resourceManager_->LoadTexture(engine->commandManager_.get(), filePath);
}
Resource::Texture* Resource::LoadTextureLongPath(const std::string& filePath) {
	return engine->resourceManager_->LoadTextureLongPath(engine->commandManager_.get(), filePath);
}


Primitive::Mesh* Resource::LoadModel(const std::string& filePath) {
	Primitive::Mesh* instance = engine->primitiveManager_->CreateInstance<Primitive::Mesh>(engine->commandManager_.get());
	instance->LoadFile(filePath);
	return instance;
}