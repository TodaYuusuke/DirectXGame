#pragma once
#include "IRenderer.h"
#include "base/directX/resource/rendering/BackBuffer.h"
#include "base/directX/resource/rendering/RenderResource.h"
#include "base/directX/resource/rendering/DepthStencil.h"
#include "base/directX/utility/DXC.h"

// 前方宣言
namespace LWP::Primitive {
	class IPrimitive;
	class Billboard2D;
	class Billboard3D;
}

namespace LWP::Base {
	/// <summary>
	/// 通常描画のレンダラー
	/// </summary>
	class NormalRenderer : public IRenderer {
	public: // ** メンバ関数 ** //
		// 通常描画のターゲット構造体
		struct Target {
			const D3D12_GPU_VIRTUAL_ADDRESS& view;	// カメラデータのView
			BackBuffer* back;	// BackBuffer（RenderRsource）
			DepthStencil* depth;	// DepthStencil
		};


		/// <summary>
		/// デフォルトコンストラクタ
		/// </summary>
		NormalRenderer();
		/// <summary>
		/// デフォルトデストラクタ
		/// </summary>
		~NormalRenderer() = default;

		/// <summary>
		/// 初期化
		/// </summary>
		void Init(GPUDevice* device, SRV* srv, RootSignature* root, DXC* dxc, std::function<void()> func);

		/// <summary>
		/// 描画命令
		/// </summary>
		/// <param name="list">コマンドリストのポインタ/param>
		/// <param name="target">ターゲット</param>
		void DrawCall(ID3D12GraphicsCommandList* list);

		/// <summary>
		/// インデックスデータを追加
		/// </summary>
		void AddIndexData(const IndexInfoStruct& data) { normal_.indexBuffer.Add(data); }
		void AddIndexDataWire(const IndexInfoStruct& data) { wireframe_.indexBuffer.Add(data); }
		void AddIndexDataBillboard2D(const IndexInfoStruct& data) { billboard2D_.indexBuffer.Add(data); }
		void AddIndexDataBillboard3D(const IndexInfoStruct& data) { billboard3D_.indexBuffer.Add(data); }

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

		// 通常
		RenderData normal_;
		// ワイヤーフレーム
		RenderData wireframe_;
		// ビルボード2D
		RenderData billboard2D_;
		// ビルボード3D
		RenderData billboard3D_;

	};
}