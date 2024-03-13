#include "ResourceManager.h"

using namespace LWP::Resource;

Manager::~Manager() {
	// XAudio2解放
	xAudio2_.Reset();
	// テクスチャ解放
	textureMap_.clear();
	// オーディオ解放
	audioMap_.clear();
	// モーション解放
	motionList_.clear();
}

void Manager::Initialize() {
	HRESULT hr;
	hr = XAudio2Create(&xAudio2_, 0, XAUDIO2_DEFAULT_PROCESSOR);
	assert(SUCCEEDED(hr));
	hr = xAudio2_->CreateMasteringVoice(&masterVoice_);
	assert(SUCCEEDED(hr));
}
void Manager::Update() {
	// モーション更新
	for (Motion* m : motionList_) {
		m->Update();
	}
}

Texture* Manager::LoadTexture(Base::DirectXCommon* directX, const std::string& filepath) {
	return LoadTextureLongPath(directX, textureDirectoryPath_ + filepath);
}
Texture* Manager::LoadTextureLongPath(Base::DirectXCommon* directX, const std::string& filepath) {
	// 読み込み済みかをチェック
	if (!textureMap_.count(filepath)) {
		// 新しいテクスチャをロード
		textureMap_[filepath] = new Texture(directX, filepath);
	}
	return textureMap_[filepath];
}

Audio* Manager::LoadAudio(const std::string& filepath) {
	return LoadAudioLongPath(audioDirectoryPath_ + filepath);
}
Audio* Manager::LoadAudioLongPath(const std::string& filepath) {
	// 読み込み済みかをチェック
	if (!audioMap_.count(filepath)) {
		// 新しいテクスチャをロード
		audioMap_[filepath] = new Audio(xAudio2_.Get(), filepath);
	}
	return audioMap_[filepath];
}

void Manager::SetMotionInstance(Motion* ptr) {
	motionList_.push_back(ptr);
}
