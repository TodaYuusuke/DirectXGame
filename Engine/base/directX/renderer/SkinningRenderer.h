#pragma once
#include "IRenderer.h"
#include "base/directX/resource/rendering/BackBuffer.h"
#include "base/directX/resource/rendering/RenderResource.h"
#include "base/directX/resource/rendering/DepthStencil.h"
#include "base/directX/utility/DXC.h"

#include "primitive/model/SkinCluster.h"
#include "primitive/model/Mesh.h"

namespace LWP::Base {
	/// <summary>
	/// スキニング描画のレンダラー
	/// </summary>
	class SkinningRenderer : public IRenderer {
	public: // ** メンバ関数 ** //
		// 通常描画のターゲット構造体
		struct Target {
			D3D12_GPU_VIRTUAL_ADDRESS view;	// カメラデータのView
			BackBuffer* back;	// BackBuffer（RenderResource）
			DepthStencil* depth;	// DepthStencil
		};


		/// <summary>
		/// デフォルトコンストラクタ
		/// </summary>
		SkinningRenderer();
		/// <summary>
		/// デフォルトデストラクタ
		/// </summary>
		~SkinningRenderer() = default;

		/// <summary>
		/// 初期化
		/// </summary>
		void Init(GPUDevice* device, SRV* srv, DXC* dxc, std::function<void()> func);

		/// <summary>
		/// 描画命令
		/// </summary>
		/// <param name="list">コマンドリストのポインタ/param>
		/// <param name="target">ターゲット</param>
		void DrawCall(ID3D12GraphicsCommandList* list);

		/// <summary>
		/// インデックスデータを追加
		/// </summary>
		void AddIndexData(const IndexInfoStruct& data) { indexBuffer.Add(data); }
		//void AddInfluenceData(const Primitive::VertexInfluence& data) { mappedInfluence.Add(data); }
		void AddMatrixPaletteData(const Primitive::WellForGPU& data) { mappedPalette.Add(data); }

		/// <summary>
		/// 描画ターゲットセット
		/// </summary>
		/// <param name="target"></param>
		void AddTarget(const Target& target) { target_.push_back(target); }

		/// <summary>
		/// インデックスデータリセット
		/// </summary>
		void Reset();

		// 頂点バッファ
		//StructuredBuffer<Primitive::SkinningVertexStruct> vertexBuffer;

	private: // ** プライベートなメンバ変数 ** //
		// ターゲット配列
		std::vector<Target> target_;

		// インデックスバッファ
		StructuredBuffer<IndexInfoStruct> indexBuffer;
		// インフルエンス
		//StructuredBuffer<Primitive::VertexInfluence> mappedInfluence;
		// MatrixPalette
		StructuredBuffer<Primitive::WellForGPU> mappedPalette;
		// RootSignature
		RootSignature root;
		// PSO
		PSO pso;
	};
}