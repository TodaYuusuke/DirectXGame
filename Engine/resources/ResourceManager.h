#pragma once
#include <map>
#include <wrl.h>

#include "texture/Texture.h"
#include "base/directX/resource/rendering/TextureResource.h"
#include "audio/Audio.h"
#include "motion/Motion.h"
//#include "model/Model.h"


#include "utility/PtrManager.h"


namespace LWP::Base {
	class DirectXCommon;
};
namespace LWP::Primitive {
	struct MeshData;
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
		const Primitive::MeshData& LoadMesh(const std::string& filepath);
		const Primitive::MeshData& LoadMeshLongPath(const std::string& filepath);

		// インスタンスのポインタをセット（ユーザー呼び出し不要）
		void SetMotionPointer(Motion* ptr) { motions_.SetPointer(ptr); }
		// インスタンスのポインタを解放（ユーザー呼び出し不要）
		void DeleteMotionPointer(Motion* ptr) { motions_.DeletePointer(ptr); }

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
		// 3Dモデルの配列
		const std::string meshDirectoryPath_ = "resources/obj/";
		std::map<std::string, Primitive::MeshData> meshDataMap_;
		// モーションの配列
		Utility::PtrManager<Motion*> motions_;


	private: // ** プライベートなメンバ変数 ** //

		// assimpによる読み込み
		Primitive::MeshData LoadAssimp(const std::string& filepath);
	};
}