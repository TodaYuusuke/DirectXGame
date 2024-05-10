#include "ResourceManager.h"

#include "math/Math.h"
#include "utility/ErrorReporter.h"
#include "base/DirectXCommon.h"

#include "component/Resource.h"

#include <fstream>
#include <sstream>


using namespace LWP;
using namespace LWP::Math;
using namespace LWP::Resource;

Manager::~Manager() {
	// XAudio2解放
	xAudio2_.Reset();
	// Media Foundation
	MFShutdown();
	// テクスチャ解放
	textureMap_.clear();
	// オーディオ解放
	audioMap_.clear();
	// モデル解放
	modelDataMap_.clear();
}

void Manager::Initialize() {
	HRESULT hr;

	// XAudio2初期化
	hr = XAudio2Create(&xAudio2_, 0, XAUDIO2_DEFAULT_PROCESSOR);
	assert(SUCCEEDED(hr));
	hr = xAudio2_->CreateMasteringVoice(&masterVoice_);
	assert(SUCCEEDED(hr));

	// MediaFoundationの初期化
	MFStartup(MF_VERSION, MFSTARTUP_NOSOCKET);
}

void Manager::Update() {
	// アニメーション更新
	for (Animation* a : animations_.list) { a->Update(); }
	// モーション更新
	for (Motion* m : motions_.list) { m->Update(); }
}
// 描画
void Manager::Draw(Base::RendererManager* render) {
	//// StructerdBufferにデータをセット済みかを保持するフラグを初期化
	//for (std::map<std::string, ModelData>::iterator it = modelDataMap_.begin(); it != modelDataMap_.end(); ++it) {
	//	it->second.isLoadedRenderer = false;
	//}

	// 必要な分モデルを描画
	for (Model* m : models_.list) { m->Draw(render, this); }
}

Texture Manager::LoadTexture(Base::DirectXCommon* directX, const std::string& filepath) {
	return LoadTextureLongPath(directX, textureDirectoryPath_ + filepath);
}
Texture Manager::LoadTextureLongPath(Base::DirectXCommon* directX, const std::string& filepath) {
	// 読み込み済みかをチェック
	if (!textureMap_.count(filepath)) {
		// 新しいテクスチャをロード
		textureMap_[filepath] = TextureStruct();
		// 読み込み
		textureMap_[filepath].r.Init(directX->GetGPUDevice(), directX->GetHeaps(), filepath);
		// インデックスを保持
		textureMap_[filepath].tex = textureMap_[filepath].r;
	}
	return textureMap_[filepath].tex;
}

AudioData* Manager::LoadAudio(const std::string& filepath) {
	return LoadAudioLongPath(audioDirectoryPath_ + filepath);
}
AudioData* Manager::LoadAudioLongPath(const std::string& filepath) {
	// 読み込み済みかをチェック
	if (!audioMap_.count(filepath)) {
		// 新しいテクスチャをロード
		audioMap_[filepath] = AudioData(xAudio2_.Get(), filepath);
	}
	return &audioMap_[filepath];
}

// モデルのデータを読み込む関数
void Manager::LoadModelData(const std::string& filePath) {
	// 読み込み済みかをチェック
	if (!modelDataMap_.count(filePath)) {
		// 読み込んだことのない3Dモデルなので読み込む
		modelDataMap_[filePath].Load(filePath);	// 要素は自動追加されるらしい;
	}
}
ModelData* Manager::GetModelData(const std::string& filePath) {
	// 読み込み済みかをチェック
	if (modelDataMap_.count(filePath)) {
		// 読み込み済みだったので返す
		return &modelDataMap_[filePath];
	}

	// 読み込めていないモデルなのでエラー
	assert(false);
	return nullptr;
}
