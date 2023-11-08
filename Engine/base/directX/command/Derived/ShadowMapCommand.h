#pragma once
#include "../ICommand.h"

namespace LWP::Base {
	/// <summary>
	/// コマンドクラス
	/// </summary>
	class ShadowMapCommand : public ICommand {
	public:
		// ** メンバ関数 ** //

		/// <summary>
		/// 描画に必要なデータ用のポインタをセットする関数
		/// </summary>
		void SetDataPtr(StructCount* structCount, DirectionLightResourceBuffer* direction, PointLightResourceBuffer* point) {
			structCount_ = structCount;
			direction_ = direction;
			point_ = point;
			dirCount = &direction->usedCount_;
			pointCount = &point->usedCount_;
		}

		/// <summary>
		/// 描画前の初期化
		/// </summary>
		void InitializePreDraw();

		/// <summary>
		/// 描画前処理
		/// </summary>
		void PreDraw(ID3D12GraphicsCommandList* list) override;

		/// <summary>
		/// 描画語処理
		/// </summary>
		void PostDraw(ID3D12GraphicsCommandList* list) override;


	private: // ** メンバ変数 ** // 

		// カウントを入れる場所のポインタ（こちらで数を１つずつ増やすことでそれぞれのviewProjectionを参照する仕組み）
		StructCount* structCount_ = nullptr;
		// 光源データのポインタ
		DirectionLightResourceBuffer* direction_ = nullptr;
		PointLightResourceBuffer* point_ = nullptr;
		// レンダリング回数の抜き出し
		UINT* dirCount = nullptr;
		UINT* pointCount = nullptr;

		// レンダリング一回で使用するデータ
		ID3D12Resource* resource_ = nullptr;	// 描画するリソース
		uint32_t index_;		// 描画するDSVのインデックス


	private: // ** プライベートな関数 ** //

		/// <summary>
		/// PSOを作成
		/// </summary>
		void CreatePSO(ID3D12Device* device, DXC* dxc, ID3D12RootSignature* rootSignature) override;
	};
}