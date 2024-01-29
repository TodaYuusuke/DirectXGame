#pragma once
#include "resources/texture/RenderTexture.h"
#include "base/directX/command/ResourceStruct.h"
#include "base/directX/resource/structuredBuffer/IStructured.h"
#include "base/directX/command/RootSignature.h"
#include "base/directX/command/PSO.h"
#include "base/directX/descriptorHeap/HeapManager.h"
#include "Config.h"


namespace LWP::Base {
	// リソースのビューを構造体に
		// 0 ... インデックスデータ
		// 1 ... 共通データ
		// 2 ... 頂点データ
		// 3 ... WorldTransform
		// 4 ... マテリアル
		// 5 ... 平行光源
		// 6 ... 点光源
		// 7 ... テクスチャ
		// 8 ... （平行光源）シャドウマップ
		// 9 ... （点光源）シャドウマップ
	struct ViewStruct {
		D3D12_GPU_DESCRIPTOR_HANDLE index;
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

	/// <summary>
	/// レンダリングコマンド
	/// </summary>
	class MainRenderer {
	public: // ** メンバ関数 ** //

		// コンストラクタ
		MainRenderer() = default;
		// デストラクタ
		~MainRenderer() = default;

		// 初期化処理
		void Init(ID3D12Device* device, ID3D12RootSignature* root, ID3D12PipelineState* pso, HeapManager* heaps);
		// Viewのリストを渡す
		void SetViewStruct(ViewStruct* viewStruct);
		// レンダリング
		void Draw(ID3D12GraphicsCommandList* list, int instanceCount);
		// データをリセット
		void Reset();

		// レンダリングするためのデータをセットする関数
		void SetRenderData(Resource::RenderTexture* target, Math::Matrix4x4 vp);


	private: // ** メンバ変数 ** //

		// レンダリングを行う先のリソース
		struct RenderData {
			Resource::RenderTexture* target;	// レンダリングを行う先のリソース
			std::unique_ptr<MatrixResourceBuffer> vpResource;	// 計算に使うViewProjection
		};
		std::unique_ptr<RenderData> renderData_;

		// ルートシグネチャのポインタ
		ID3D12RootSignature* root_;
		// PSOのポインタ
		ID3D12PipelineState* pso_;
		// レンダリングにつかうリソースをまとめた構造体
		ViewStruct* viewStruct_;
		// ディスクリプタヒープ管理クラスのポインタ
		HeapManager* heaps_ = nullptr;


	private: // ** メンバ変数 ** //

		void PreDraw(ID3D12GraphicsCommandList* list);
		void PostDraw(ID3D12GraphicsCommandList* list);
	};
}