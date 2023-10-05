#pragma once
#include <map>

#include "texture/Texture.h"
//#include "audio/Audio.h"
//#include "model/Model.h"

namespace LWP::Resource {
	/// <summary>
	/// 読み込み済みのリソースを管理するクラス
	/// </summary>
	class Manager final {
	public:
		Manager() = default;
		~Manager() = default;

		// 初期化
		void Initialize();

		// 読み込み
		const Texture* LoadTexture(Base::CommandManager* manager, const std::string& filepath);
		//Audio LoadAudio(const std::string& filepath);
		//Model LoadModel(const std::string& filepath);

	private: // 各種リソース

		// テクスチャの配列
		std::map<std::string, Texture*> textureMap_;
		// オーディオの配列
		//std::map<std::string, Audio> audioMap_;
		// 3Dモデルの配列
		//std::map<std::string, Model> modelMap_;
	};
}