#include "ResourceManager.h"

using namespace LWP::Resource;

void Manager::Initialize() {
	textureMap_.clear();
}

Texture* Manager::LoadTexture(Base::CommandManager* manager, const std::string& filepath) {
	// 読み込み済みかをチェック
	if (!textureMap_.count(filepath)) {
		// 新しいテクスチャをロード
		textureMap_[filepath] = new Texture(manager, textureDirectoryPath + filepath);
	}
	return textureMap_[filepath];
}
//Audio Manager::LoadAudio(const std::string& filepath);
//Model Manager::LoadModel(const std::string& filepath);