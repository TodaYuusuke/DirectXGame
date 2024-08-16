#pragma once
#include "IRenderer.h"
#include "base/directX/resource/rendering/BackBuffer.h"
#include "base/directX/resource/rendering/RenderResource.h"
#include "base/directX/resource/rendering/DepthStencil.h"
#include "base/directX/utility/DXC.h"

#include "resources/model/RigidModel.h"

namespace LWP::Base {
	/// <summary>
	/// メッシュシェーダーによるレンダラー
	/// </summary>
	class MeshRenderer : public IRenderer {
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
		MeshRenderer() = default;
		/// <summary>
		/// デフォルトデストラクタ
		/// </summary>
		~MeshRenderer() = default;

		/// <summary>
		/// 初期化
		/// </summary>
		void Init(GPUDevice* device, SRV* srv, DXC* dxc, std::function<void()> func);

		/// <summary>
		/// 描画命令
		/// </summary>
		/// <param name="list">コマンドリストのポインタ/param>
		/// <param name="target">ターゲット</param>
		void DrawCall(ID3D12GraphicsCommandList6* list);

		/// <summary>
		/// 描画ターゲットセット
		/// </summary>
		/// <param name="target"></param>
		void AddTarget(const Target& target) { target_.push_back(target); }
		/// <summary>
		/// 草描画用データセット
		/// </summary>
		void AddGrassData(const D3D12_GPU_DESCRIPTOR_HANDLE& view, int count) {
			grassData_.positionView = view;
			grassData_.count = count;
		}

		/// <summary>
		/// リセット
		/// </summary>
		void Reset();


	private: // ** プライベートなメンバ変数 ** //
		// srvのポインタを保持
		SRV* srv_;

		// ターゲット配列
		std::vector<Target> target_;
		
		// 書き込むデータの種類
		struct RenderData {
			// RootSignature
			RootSignature root;
			// PSO
			PSO pso;
			// PSO（WireFrame）
			PSO wirePso;
		};

		// 固体モデル
		RenderData rigid_;
		// スキニングモデル
		RenderData skinning_;
		// 静的モデル
		RenderData static_;
		// 環境モデル
		RenderData eMap_;

		// 草の数用データ
		struct GrassData {
			RootSignature root;
			PSO pso;
			int count;
			D3D12_GPU_DESCRIPTOR_HANDLE positionView;
		}grassData_;

	private: // ** プライベートなメンバ関数 ** //

		/// <summary>
		/// 全てのモデルをターゲットに描画する関数
		/// </summary>
		void DispatchAllModel(ID3D12GraphicsCommandList6* list, D3D12_GPU_VIRTUAL_ADDRESS cameraView);
	};
}