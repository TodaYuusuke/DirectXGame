#include "Resource.h"

using namespace LWP::System;
using namespace LWP;

Resource::Texture Resource::LoadTexture(const std::string& filePath) {
	return Resource::Manager::GetInstance()->LoadTexture(filePath);
}
Resource::Texture Resource::LoadTextureLongPath(const std::string& filePath) {
	return Resource::Manager::GetInstance()->LoadTextureLongPath(filePath);
}


Resource::AudioData* Resource::LoadAudio(const std::string& filePath) {
	return Resource::Manager::GetInstance()->LoadAudio(filePath);
}
Resource::AudioData* Resource::LoadAudioLongPath(const std::string& filePath) {
	return Resource::Manager::GetInstance()->LoadAudioLongPath(filePath);
}

void Resource::LoadModel(const std::string& filePath) {
	Resource::Manager::GetInstance()->LoadModelData(filePath);
}
Resource::ModelData* Resource::GetModel(const std::string& filePath) {
	return Resource::Manager::GetInstance()->GetModelData(filePath);
}

