#pragma once
#include "ResourceStruct.h"
#include "base/directX/utility/RootSignature.h"
#include "base/directX/resource/data/ConstantBuffer.h"
#include "base/directX/resource/data/StructuredBuffer.h"



namespace LWP::Base {
	// 前方宣言
	struct DirectionalLightStruct;
	struct PointLightStruct;

	/// <summary>
	/// レンダラー用のリソースバッファーをまとめたクラス
	/// </summary>
	class BufferGroup {
	public: // ** メンバ関数 ** //

		/// <summary>
		/// デフォルトコンストラクタ
		/// </summary>
		BufferGroup() = default;
		/// <summary>
		/// デフォルトデストラクタ
		/// </summary>
		~BufferGroup() = default;


		/// <summary>
		/// 初期化
		/// </summary>
		void Init(GPUDevice* device, SRV* srv);

		/// <summary>
		/// RootSignatureのポインタを返す
		/// </summary>
		RootSignature* GetRoot() { return root_.get(); }

		// コマンドリストにviewをセットする
		void SetCommonView(int num, ID3D12GraphicsCommandList* list);
		void SetVertexView(int num, ID3D12GraphicsCommandList* list);
		void SetTransformView(int num, ID3D12GraphicsCommandList* list);
		void SetMaterialView(int num, ID3D12GraphicsCommandList* list);
		void SetDirLightView(int num, ID3D12GraphicsCommandList* list);
		void SetPointLightView(int num, ID3D12GraphicsCommandList* list);

		/// <summary>
		/// 共通データのポインタをゲット
		/// </summary>
		CommonStruct* GetCommonData() { return common_->data_; }
		/// <summary>
		/// 頂点データの数を取得
		/// </summary>
		int GetVertexCount() { return vertex_->GetCount(); }

		/// <summary>
		/// データをセット
		/// </summary>
		int AddData(const VertexStruct& data);
		int AddData(const WTFStruct& data);
		int AddData(const MaterialStruct& data);
		int AddData(const DirectionalLightStruct& data);
		int AddData(const PointLightStruct& data);

		// 各カウントリセット
		void Reset();

	private: // ** メンバ変数 ** //
		// RootSignature
		std::unique_ptr<RootSignature> root_ = nullptr;

		// 全描画で共通のデータ
		std::unique_ptr<ConstantBuffer<CommonStruct>> common_ = nullptr;
		
		// 頂点データ
		std::unique_ptr<StructuredBuffer<VertexStruct>> vertex_ = nullptr;
		// WorldTransformデータ
		std::unique_ptr<StructuredBuffer<WTFStruct>> transform_ = nullptr;
		// マテリアルデータ
		std::unique_ptr<StructuredBuffer<MaterialStruct>> material_ = nullptr;

		// 平行光源
		std::unique_ptr<StructuredBuffer<DirectionalLightStruct>> directionLight_ = nullptr;
		// 点光源
		std::unique_ptr<StructuredBuffer<PointLightStruct>> pointLight_ = nullptr;

	};
}