#pragma once
#include <map>
#include <wrl.h>

#include "texture/Texture.h"
#include "base/directX/resource/rendering/TextureResource.h"
#include "audio/Audio.h"
#include "motion/Animation.h"
#include "motion/Motion.h"

// 3Dモデル
#include "primitive/3d/OldMesh.h"
#include "model/Model.h"
#include "model/ModelData.h"


#include "utility/PtrManager.h"


namespace LWP::Base {
	class DirectXCommon;
};

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
		// 更新
		void Update();
		// 描画
		void Draw(Base::RendererManager* render);

		// 読み込み

		/// <summary>
		/// テクスチャを読み込む関数
		/// </summary>
		/// <param name="manager">マテリアル等のリソース作成用</param>
		/// <param name="filepath">"resources/texture/" より後のファイルパス</param>
		/// <returns></returns>
		Texture LoadTexture(Base::DirectXCommon* directX, const std::string& filepath);
		Texture LoadTextureLongPath(Base::DirectXCommon* directX, const std::string& filepath);
		AudioData* LoadAudio(const std::string& filepath);
		AudioData* LoadAudioLongPath(const std::string& filepath);
		Primitive::OldMeshData* LoadOldMesh(const std::string& filepath);
		Primitive::OldMeshData* LoadOldMeshLongPath(const std::string& filepath);

		// モデルのデータを読み込む関数
		void LoadModelData(const std::string& filePath);
		// モデルデータの実体を受け取る関数
		ModelData* GetModelData(const std::string& filePath);


		// インスタンスのポインタをセットする関数群（ユーザー呼び出し不要）
		void SetPointer(Model* ptr) { models_.SetPointer(ptr); }
		void SetPointer(Animation* ptr) { animations_.SetPointer(ptr); }
		void SetPointer(Motion* ptr) { motions_.SetPointer(ptr); }
		// インスタンスのポインタを解放する関数群（ユーザー呼び出し不要）
		void DeletePointer(Model* ptr) { models_.DeletePointer(ptr); }
		void DeletePointer(Animation* ptr) { animations_.DeletePointer(ptr); }
		void DeletePointer(Motion* ptr) { motions_.DeletePointer(ptr); }

	private: // 各種リソース

		// オーディオ用のオブジェクト
		Microsoft::WRL::ComPtr<IXAudio2> xAudio2_;
		IXAudio2MasteringVoice* masterVoice_;

		// テクスチャの配列
		struct TextureStruct {
			Texture tex;
			Base::TextureResource r;
		};
		const std::string textureDirectoryPath_ = "resources/texture/";
		std::map<std::string, TextureStruct> textureMap_;
		// オーディオの配列
		const std::string audioDirectoryPath_ = "resources/audio/";
		std::map<std::string, AudioData> audioMap_;
		// 古いmeshの配列
		const std::string oldMeshDirectoryPath_ = "resources/model/";
		std::map<std::string, Primitive::OldMeshData> oldMeshMap_;

		// 3Dモデルの配列
		std::map<std::string, ModelData> modelDataMap_;
		// 3Dモデルのアダプター
		Utility::PtrManager<Model*> models_;

		// アニメーションの配列
		Utility::PtrManager<Animation*> animations_;
		// モーションの配列
		Utility::PtrManager<Motion*> motions_;

#if DEMO
		// ImGui用変数
		int currentItem = 0;
#endif

	private:
		Primitive::OldMeshData LoadAssimp(const std::string& filepath);
	};
}