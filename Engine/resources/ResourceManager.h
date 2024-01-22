#pragma once
#include <map>
#include <wrl.h>

#include "texture/Texture.h"
#include "audio/Audio.h"
//#include "model/Model.h"

namespace LWP::Resource {
	/// <summary>
	/// 読み込み済みのリソースを管理するクラス
	/// </summary>
	class Manager final {
	public:
		Manager() = default;
		~Manager();

		// 初期化
		void Initialize();

		// 読み込み
		
		/// <summary>
		/// テクスチャを読み込む関数
		/// </summary>
		/// <param name="manager">マテリアル等のリソース作成用</param>
		/// <param name="filepath">"resources/texture/" より後のファイルパス</param>
		/// <returns></returns>
		Texture* LoadTexture(Base::DirectXCommon* directX, const std::string& filepath);
		Texture* LoadTextureLongPath(Base::DirectXCommon* directX, const std::string& filepath);
		Audio* LoadAudio(const std::string& filepath);
		Audio* LoadAudioLongPath(const std::string& filepath);
		//Model LoadModel(const std::string& filepath);

	private: // 各種リソース

		// オーディオ用のオブジェクト
		Microsoft::WRL::ComPtr<IXAudio2> xAudio2_;
		IXAudio2MasteringVoice* masterVoice_;

		// テクスチャの配列
		const std::string textureDirectoryPath_ = "resources/texture/";
		std::map<std::string, Texture*> textureMap_;
		// オーディオの配列
		const std::string audioDirectoryPath_ = "resources/audio/";
		std::map<std::string, Audio*> audioMap_;
		// 3Dモデルの配列
		const std::string modelDirectoryPath_ = "resources/obj/";
		//std::map<std::string, Model> modelMap_;
	};
}