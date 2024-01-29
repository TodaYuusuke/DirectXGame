#pragma once
#include "MainRenderer.h"


namespace LWP::Base {
	/// <summary>
	/// レンダリングコマンド
	/// </summary>
	class Renderer {
	public: // ** メンバ関数 ** //

		// コンストラクタ
		Renderer() = default;
		// デストラクタ
		~Renderer() = default;

		// 初期化処理
		void Init(ID3D12Device* device, DXC* dxc, HeapManager* heaps);
		// サブをレンダリング
		//void SubDraw(ID3D12GraphicsCommandList* list, int instanceCount);
		// メインをレンダリング
		void MainDraw(ID3D12GraphicsCommandList* list, int instanceCount);
		// データをリセット
		void Reset();

		// レンダリングするためのデータをセットする関数
		//void SetSubRenderData(Resource::RenderTexture* target, Math::Matrix4x4 vp);
		void SetMainRenderData(Resource::RenderTexture* target, Math::Matrix4x4 vp);


	private: // ** メンバ変数 ** //

		std::unique_ptr<MainRenderer> mainRenderer_;
		
		// サブレンダリング用
		//struct SubRenderData {
		//	std::unique_ptr<RenderData> renderData[lwpC::Rendering::kMaxMultiWindowRendering];	// 計算に使うViewProjection
		//	std::unique_ptr<IStructured<IndexInfoStruct>> indexInfo;
		//};
		//std::unique_ptr<SubRenderData> subRenderData_;


		// ルートシグネチャ
		std::unique_ptr<RootSignature> root_;
		// PSO
		std::unique_ptr<PSO> pso_;
		// ディスクリプタヒープ管理クラスのポインタ
		HeapManager* heaps_ = nullptr;

		// カウンター
		Utility::Counter counter_;

	private: // ** メンバ変数 ** //

		void PreDraw(ID3D12GraphicsCommandList* list, int index);
		void PostDraw(ID3D12GraphicsCommandList* list, int index);
		void PostMainDraw(ID3D12GraphicsCommandList* list, int index);
	};
}