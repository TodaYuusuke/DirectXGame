#include "Resource.h"

using namespace LWP::System;
using namespace LWP;

Resource::Texture* Resource::LoadTexture(const std::string& filePath) {
	return engine->resourceManager_->LoadTexture(engine->directXCommon_.get(), filePath);
}
Resource::Texture* Resource::LoadTextureLongPath(const std::string& filePath) {
	return engine->resourceManager_->LoadTextureLongPath(engine->directXCommon_.get(), filePath);
}


Resource::Audio* Resource::LoadAudio(const std::string& filePath) {
	return engine->resourceManager_->LoadAudio(filePath);
}
Resource::Audio* Resource::LoadAudioLongPath(const std::string& filePath) {
	return engine->resourceManager_->LoadAudioLongPath(filePath);
}


Primitive::Mesh* Resource::LoadModel(const std::string& filePath) {
	Primitive::Mesh* instance = engine->primitiveManager_->CreateInstance<Primitive::Mesh>(engine->directXCommon_->GetCommandManager());
	instance->LoadFile(filePath);
	return instance;
}