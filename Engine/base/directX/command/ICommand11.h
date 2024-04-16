#pragma once
#include "../utility/GPUDevice.h"
#include "ResourceStruct.h"
#include "../utility/PSO.h"

#include "../resource/structuredBuffer/IStructured.h"

#include <memory>

namespace LWP::Base {
	class RTV;
	class SRV;
	class DSV;

	/// <summary>
	/// RenderTargetView
	/// </summary>
	class ICommand {
	public: // ** パブリックなメンバ変数 ** //
		// リソースのビューを構造体に
		// 0 ... 共通データ
		// 1 ... 頂点データ
		// 2 ... WorldTransform
		// 3 ... マテリアル
		// 4 ... 平行光源
		// 5 ... 点光源
		// 6 ... テクスチャ
		// 7 ... （平行光源）シャドウマップ
		// 8 ... （点光源）シャドウマップ
		struct ViewStruct {
			D3D12_GPU_VIRTUAL_ADDRESS commonData;
			D3D12_GPU_DESCRIPTOR_HANDLE vertex;
			D3D12_GPU_DESCRIPTOR_HANDLE wtf;
			D3D12_GPU_DESCRIPTOR_HANDLE material;
			D3D12_GPU_DESCRIPTOR_HANDLE directionLight;
			D3D12_GPU_DESCRIPTOR_HANDLE pointLight;
			D3D12_GPU_DESCRIPTOR_HANDLE texture;
			D3D12_GPU_DESCRIPTOR_HANDLE directionShadowMap;
			D3D12_GPU_DESCRIPTOR_HANDLE pointShadowMap;
		};


		// -- レンダリング先のハンドル -- //
		
		// RTVのハンドル
		//D3D12_CPU_DESCRIPTOR_HANDLE* rtvHandle_ = nullptr;
		// DSVのハンドル
		//D3D12_CPU_DESCRIPTOR_HANDLE* dsvHandle_ = nullptr;
		

		// -- リソースバリア -- //
		
		// 描画前に設定するバリア
		D3D12_RESOURCE_BARRIER preBarrier_;
		// 描画後に設定するバリア
		D3D12_RESOURCE_BARRIER postBarrier_;

	public: // ** メンバ関数 ** //

		ICommand() = default;
		~ICommand() = default;

		/// <summary>
		/// 初期化
		/// </summary>
		void Initialize(ID3D12Device* device, DXC* dxc, ID3D12RootSignature* rootSignature);
		/// <summary>
		/// 派生初期化
		/// </summary>
		virtual void DerivedInitialize();

		// 描画用のデータをセット
		virtual void SetDrawData(IndexInfoStruct data);

		/// <summary>
		/// 描画処理
		/// </summary>
		/// <param name="viewStruct">全てのviewの構造体</param>
		virtual void Draw(ID3D12RootSignature* rootSignature, ID3D12GraphicsCommandList* list, ViewStruct viewStruct);

		/// <summary>
		/// ディスクリプタヒープのポインタをセットする関数
		/// </summary>
		void SetDescriptorHeap(RTV* rtv, DSV* dsv, SRV* srv) {
			rtv_ = rtv;
			dsv_ = dsv;
			srv_ = srv;
		}

		/// <summary>
		/// 描画後のリセット処理
		/// </summary>
		void Reset() { indexData_->Reset(); }


		/// <summary>
		/// リソースバリアの実態を作る関数（今だけ静的関数）
		/// </summary>
		static D3D12_RESOURCE_BARRIER MakeResourceBarrier(ID3D12Resource*, D3D12_RESOURCE_STATES, D3D12_RESOURCE_STATES);


	protected: // ** メンバ変数 ** //

		// ディスクリプタヒープのポインタ
		RTV* rtv_ = nullptr;
		DSV* dsv_ = nullptr;
		SRV* srv_ = nullptr;

		// グラフィックパイプラインの状態を定義
		std::unique_ptr<PSO> pso_;


		// -- 描画用リソース -- //

		// インデックス情報の構造体
		std::unique_ptr<IStructured<IndexInfoStruct>> indexData_;
		// 計算に使うViewProjection
		std::unique_ptr<MatrixResourceBuffer> vpResourceBuffer_;


	protected: // ** プライベートな関数 ** //

		/// <summary>
		/// 描画前処理
		/// </summary>
		virtual void PreDraw(ID3D12GraphicsCommandList* list);

		/// <summary>
		/// 描画語処理
		/// </summary>
		virtual void PostDraw(ID3D12GraphicsCommandList* list);

		/// <summary>
		/// PSOを作成
		/// </summary>
		virtual void CreatePSO(ID3D12Device* device, DXC* dxc, ID3D12RootSignature* rootSignature);
	};
}