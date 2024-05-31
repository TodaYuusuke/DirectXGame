#pragma once
#include "IModel.h"

namespace LWP::Resource {
	/// <summary>
	/// 3Dモデルを扱うアダプタークラス
	/// </summary>
	class SkinningModel final : public IModel {
	public: // ** パブリックなメンバ変数 ** //
		
		// スケルトン
		Primitive::Skeleton skeleton{};
		// スキンクラスター
		std::optional<Primitive::SkinCluster> skinCluster{};
		
		// バッファー
		//Base::ConstantBuffer<Base::InstanceSkinData> buffer;
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

		/// <summary>
		/// 埋め立てかワイヤーフレームで描画するかを切り替える
		/// </summary>
		void ChangeFillMode();
	};
}