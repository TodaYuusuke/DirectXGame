#pragma once
#include "IModel.h"

// 前方宣言
namespace LWP::Resource {
	class SkinningModel;
}

// モデル別のデータの構造体（AS実装のときに役立つ予定）
namespace LWP::Base {
	struct InstanceSkinData {
		Base::WTFStruct wtf;
		int32_t enableLighting;

		InstanceSkinData() = default;
		InstanceSkinData(const Resource::SkinningModel& value);
		// Materialクラスのデータを代入する演算子をオーバーロード
		InstanceSkinData& operator=(const Resource::SkinningModel& value);
	};
}

namespace LWP::Resource {
	/// <summary>
	/// 3Dモデルを扱うアダプタークラス
	/// </summary>
	class SkinningModel final : public IModel {
	public: // ** パブリックなメンバ変数 ** //
		
		// スケルトン
		std::optional<Primitive::Skeleton> skeleton{};
		// スキンクラスター
		std::optional<Primitive::SkinCluster> skinCluster{};
		
		// バッファー
		Base::ConstantBuffer<Base::InstanceSkinData> buffer;
		std::unique_ptr<Base::StructuredBuffer<Primitive::WellForGPU>> wellBuffer;

	public: // ** メンバ関数 ** //

		/// <summary>
		/// デフォルトコンストラクタ
		/// </summary>
		SkinningModel();
		/// <summary>
		/// デフォルトデストラクタ
		/// </summary>
		~SkinningModel() override;

		/// <summary>
		/// 3Dモデルのデータを読み込む（exeからのパス指定）
		/// </summary>
		/// <param name="filePath">読み込むファイルの名前</param>
		void LoadFullPath(const std::string& filePath) override;

		/// <summary>
		/// 更新（ユーザー呼び出し禁止）
		/// </summary>
		void Update() override;

		/// <summary>
		/// デバッグ用ImGui
		/// </summary>
		void DebugGUI() override;
	};
}