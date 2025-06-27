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
#include "model/RigidModel.h"
#include "model/SkinningModel.h"
#include "model/StaticModel.h"
#include "model/EMapModel.h"
#include "model/ModelData.h"

#include "level/LevelData.h"

#include "utility/PtrManager.h"


namespace LWP::Base {
	class DirectXCommon;
};
// モデル別のデータの構造体（AS実装のときに役立つ予定）
namespace LWP::Base {
	struct InstanceData {
		Base::WTFStruct wtf;

		InstanceData() = default;
		InstanceData(const Resource::RigidModel& value);
		InstanceData(const Resource::SkinningModel& value);
		InstanceData(const Resource::EMapModel& value);
		// Materialクラスのデータを代入する演算子をオーバーロード
		InstanceData& operator=(const Resource::RigidModel& value);
		InstanceData& operator=(const Resource::SkinningModel& value);
		InstanceData& operator=(const Resource::EMapModel& value);
	};
}


namespace LWP::Resource {
	// ** 使用する構造体を先に宣言しておく ** //
	
	// モデルデータ本体とアダプタークラスのポインタマネージャー
	struct Models {
		struct Common {
			uint32_t instanceCount = 0;
			uint32_t materialCount = 0;
			uint32_t jointCount = 0;
		};
		struct RigidBuffer {
			// トランスフォームのバッファー
			std::unique_ptr<Base::StructuredBuffer<Base::InstanceData>> inst;
			// マテリアルバッファー
			std::unique_ptr<Base::StructuredBuffer<Base::MaterialStruct>> material;
			// モデル別共通データ
			Base::ConstantBuffer<Common> common;

			void Init();
			void Reset(uint32_t mSize);
		};
		struct SkinBuffer {
			// トランスフォームのバッファー
			std::unique_ptr<Base::StructuredBuffer<Base::InstanceData>> inst;
			// マテリアルバッファー
			std::unique_ptr<Base::StructuredBuffer<Base::MaterialStruct>> material;
			// WellForGPUバッファー
			std::unique_ptr<Base::StructuredBuffer<Primitive::WellForGPU>> well;
			// モデル別共通データ
			Base::ConstantBuffer<Common> common;

			void Init();
			void Reset(uint32_t mSize);
		};
		template<typename T, typename U>
		struct Pointers {
			Utility::PtrManager<T*> ptrs;
			U buffer;

			void Init() { buffer.Init(); }
			void Reset(uint32_t mSize) { buffer.Reset(mSize); }
		};
		
		template<typename T, typename U>
		struct FillMode {
			Pointers<T, U> solid;
			Pointers<T, U> wireFrame;

			void Init() {
				solid.Init();
				wireFrame.Init();
			}
			void Reset(uint32_t mSize) {
				solid.Reset(mSize);
				wireFrame.Reset(mSize);
			}
		};

		ModelData data;
		
		FillMode<RigidModel, RigidBuffer> rigid;
		FillMode<SkinningModel, SkinBuffer> skin;
		Utility::PtrManager<StaticModel*> statics;
		Pointers<EMapModel, RigidBuffer> eMaps;
	};


	/// <summary>
	/// 読み込み済みのリソースを管理するクラス
	/// </summary>
	class Manager : public Utility::ISingleton<Manager> {
		friend class Utility::ISingleton<Manager>;	// ISingletonをフレンドクラスにしてコンストラクタを呼び出せるように
	private: // ** シングルトン化に必要な処理 ** //

		/// <summary>
		/// コンストラクタをプライベートに
		/// </summary>
		Manager() = default;

	public: // メンバ関数

		/// <summary>
		/// デストラクタ
		/// </summary>
		~Manager();

		// 初期化
		void Init();
		// 更新
		void Update();

		// 読み込み

		/// <summary>
		/// テクスチャを読み込む関数
		/// </summary>
		/// <param name="manager">マテリアル等のリソース作成用</param>
		/// <param name="filepath">"resources/texture/" より後のファイルパス</param>
		/// <returns></returns>
		Texture LoadTexture(const std::string& filepath);
		Texture LoadTextureLongPath(const std::string& filepath);
		AudioData* LoadAudio(const std::string& filepath);
		AudioData* LoadAudioLongPath(const std::string& filepath);
		Primitive::OldMeshData* LoadOldMesh(const std::string& filepath);
		Primitive::OldMeshData* LoadOldMeshLongPath(const std::string& filepath);

		// モデルのデータを読み込む関数
		void LoadModelData(const std::string& filePath);
		// モデルデータの実体を受け取る関数
		ModelData* GetModelData(const std::string& filePath);
		// モデルデータ達の配列を受け取る関数
		std::vector<std::reference_wrapper<Models>> GetModels();

		// インスタンスのポインタをセットする関数群（ユーザー呼び出し不要）
		void SetPointer(RigidModel* ptr, const std::string& filePath) { modelDataMap_[filePath].rigid.solid.ptrs.SetPtr(ptr); }
		void SetPointer(SkinningModel* ptr, const std::string& filePath) { modelDataMap_[filePath].skin.solid.ptrs.SetPtr(ptr); }
		void SetPointer(StaticModel* ptr, const std::string& filePath) { modelDataMap_[filePath].statics.SetPtr(ptr); }
		void SetPointer(EMapModel* ptr, const std::string& filePath) { modelDataMap_[filePath].eMaps.ptrs.SetPtr(ptr); }
		void SetPointer(Animation* ptr) { animations_.SetPtr(ptr); }
		void SetPointer(Motion* ptr) { motions_.SetPtr(ptr); }
		// インスタンスのポインタを解放する関数群（ユーザー呼び出し不要）
		void DeletePointer(RigidModel* ptr, const std::string& filePath) {
			modelDataMap_[filePath].rigid.solid.ptrs.DeletePtr(ptr);
			modelDataMap_[filePath].rigid.wireFrame.ptrs.DeletePtr(ptr);
		}
		void DeletePointer(SkinningModel* ptr, const std::string& filePath) {
			modelDataMap_[filePath].skin.solid.ptrs.DeletePtr(ptr);
			modelDataMap_[filePath].skin.wireFrame.ptrs.DeletePtr(ptr);
		}
		void DeletePointer(StaticModel* ptr, const std::string& filePath) { modelDataMap_[filePath].statics.DeletePtr(ptr); }
		void DeletePointer(EMapModel* ptr, const std::string& filePath) { modelDataMap_[filePath].eMaps.ptrs.DeletePtr(ptr); }
		void DeletePointer(Animation* ptr) { animations_.DeletePtr(ptr); }
		void DeletePointer(Motion* ptr) { motions_.DeletePtr(ptr); }

		// 特別な関数
		void ChangeFillMode(RigidModel* ptr, const std::string& filePath);
		void ChangeFillMode(SkinningModel* ptr, const std::string& filePath);


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
		std::map<std::string, Models> modelDataMap_;

		// アニメーションの配列
		Utility::PtrManager<Animation*> animations_;
		// モーションの配列
		Utility::PtrManager<Motion*> motions_;


	private:
		Primitive::OldMeshData LoadAssimp(const std::string& filepath);

		void RigidGUI(Models& m);
		void SkinningGUI(Models& m);
		void StaticGUI(Models& m);


	private:
		// ImGui用変数
		int selectedClass = 0;
		int radioValue = 0;
		int currentItem = 0;
		int currentAnim = 0;
		// デバッグ用の生成したインスンタンスを格納しておく配列
		std::vector<IModel*> debugModels;
	public:
		/// <summary>
		/// Debug用GUI
		/// </summary>
		void DebugGUI();
	};
}