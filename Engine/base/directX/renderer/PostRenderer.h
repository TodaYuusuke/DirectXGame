#pragma once
#include "IRenderer.h"
#include "base/directX/resource/rendering/BackBuffer.h"
#include "base/directX/resource/rendering/RenderResource.h"
#include "base/directX/resource/rendering/DepthStencil.h"
#include "base/directX/utility/DXC.h"

namespace LWP::Base {
	/// <summary>
	/// ポストプロセスのあとに描画するレンダラー
	/// </summary>
	class PostRenderer : public IRenderer {
	public: // ** メンバ関数 ** //
		// 通常描画のターゲット構造体
		struct Target {
			D3D12_GPU_VIRTUAL_ADDRESS view;	// カメラデータのView
			BackBuffer* back;	// BackBuffer（RenderRsource）
			DepthStencil* depth;	// DepthStencil
		};


		/// <summary>
		/// デフォルトコンストラクタ
		/// </summary>
		PostRenderer();
		/// <summary>
		/// デフォルトデストラクタ
		/// </summary>
		~PostRenderer() = default;

		/// <summary>
		/// 初期化
		/// </summary>
		void Init(RootSignature* root, std::function<void()> func);

		/// <summary>
		/// 描画命令
		/// </summary>
		/// <param name="list">コマンドリストのポインタ/param>
		/// <param name="target">ターゲット</param>
		void DrawCall(ID3D12GraphicsCommandList* list);

		/// <summary>
		/// インデックスデータを追加
		/// </summary>
		void AddIndexDataSprite(const IndexInfoStruct& data) { sprite_.indexBuffer.Add(data); }

		/// <summary>
		/// 描画ターゲットセット
		/// </summary>
		/// <param name="target"></param>
		void AddTarget(const Target& target) { target_.push_back(target); }

		/// <summary>
		/// インデックスデータリセット
		/// </summary>
		void Reset();


	private: // ** プライベートなメンバ変数 ** //
		// ターゲット配列
		std::vector<Target> target_;
		
		// 書き込むデータの種類
		struct RenderData {
			// インデックスバッファ
			StructuredBuffer<IndexInfoStruct> indexBuffer;
			// PSO
			PSO pso;

			RenderData() = delete;
			RenderData(uint32_t size) : indexBuffer(size) {};
		};

		// スプライト
		RenderData sprite_;
	};
}