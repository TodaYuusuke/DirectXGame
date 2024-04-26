#include "Resource.h"

using namespace LWP::System;
using namespace LWP;

Resource::Texture Resource::LoadTexture(const std::string& filePath) {
	return engine->resourceManager_->LoadTexture(engine->directXCommon_.get(), filePath);
}
Resource::Texture Resource::LoadTextureLongPath(const std::string& filePath) {
	return engine->resourceManager_->LoadTextureLongPath(engine->directXCommon_.get(), filePath);
}


Resource::AudioData* Resource::LoadAudio(const std::string& filePath) {
	return engine->resourceManager_->LoadAudio(filePath);
}
Resource::AudioData* Resource::LoadAudioLongPath(const std::string& filePath) {
	return engine->resourceManager_->LoadAudioLongPath(filePath);
}


const Primitive::MeshData& Resource::LoadMesh(const std::string& filePath) {
	return engine->resourceManager_->LoadMesh(filePath);
}
const Primitive::MeshData& Resource::LoadMeshLongPath(const std::string& filePath) {
	return engine->resourceManager_->LoadMeshLongPath(filePath);
}

void Resource::SetInstance(Resource::Animation* ptr) { engine->resourceManager_->SetPointer(ptr); }
void Resource::SetInstance(Resource::Motion* ptr) { engine->resourceManager_->SetPointer(ptr); }
void Resource::DeleteInstance(Resource::Animation* ptr) { engine->resourceManager_->DeletePointer(ptr); }
void Resource::DeleteInstance(Resource::Motion* ptr) { engine->resourceManager_->DeletePointer(ptr); }

