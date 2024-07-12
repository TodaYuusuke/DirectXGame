#pragma once
#include "IModel.h"

namespace LWP::Resource {
	/// <summary>
	/// 実行中に動かすことのない3Dモデル
	/// </summary>
	class StaticModel : public IModel {
	public: // ** メンバ変数 ** //
		// 加工済み頂点
		std::unique_ptr<Base::StructuredBuffer<Base::OutputVertexStruct>> vertexBuffer_;

	public: // ** メンバ関数 ** //

		/// <summary>
		/// デフォルトコンストラクタ
		/// </summary>
		StaticModel();
		/// <summary>
		/// デフォルトデストラクタ
		/// </summary>
		~StaticModel() override;

		/// <summary>
		/// 3Dモデルのデータを読み込む（exeからのパス指定）
		/// </summary>
		/// <param name="filePath">読み込むファイルの名前</param>
		void LoadFullPath(const std::string& filePath) override;
		/// <summary>
		/// ワールド変換行列を適応する
		/// </summary>
		void ApplyWorldTransform(const Object::TransformQuat& wtf);

		/// <summary>
		/// デバッグ用ImGui
		/// </summary>
		void DebugGUI() override;
		/// <summary>
		/// 頂点バッファのViewを返す関数
		/// </summary>
		D3D12_GPU_DESCRIPTOR_HANDLE GetVertexBufferView() const { return vertexBuffer_->GetGPUView(); }

	private: // ** 秘匿する関数たち ** //

		/// <summary>
		/// 更新（ユーザー呼び出し禁止）
		/// </summary>
		void Update() override {};
		/// <summary>
		/// 埋め立てかワイヤーフレームで描画するかを切り替える
		/// </summary>
		void ChangeFillMode() override {};
	};
}