#pragma once
#include "IRenderer.h"
#include "base/directX/utility/DXC.h"

#include "../resource/rendering/CubeMap.h"

#include <vector>

// 前方宣言
namespace LWP::Primitive {
	class IPrimitive;
	class Billboard2D;
	class Billboard3D;
}

namespace LWP::Base {
	/// <summary>
	/// 環境マップのレンダラー
	/// </summary>
	class EnvironmentMapRenderer : public IRenderer {
	public: // ** メンバ関数 ** //
		
		/// <summary>
		/// デフォルトコンストラクタ
		/// </summary>
		EnvironmentMapRenderer();
		/// <summary>
		/// デフォルトデストラクタ
		/// </summary>
		~EnvironmentMapRenderer() = default;

		/// <summary>
		/// 初期化
		/// </summary>
		void Init(GPUDevice* device, SRV* srv, DXC* dxc, std::function<void()> func);

		/// <summary>
		/// 描画命令（平行光源）
		/// </summary>
		/// <param name="list">コマンドリストのポインタ/param>
		void DrawCall(ID3D12GraphicsCommandList6* list);

		/// <summary>
		/// インデックスデータリセット
		/// </summary>
		void Reset();


	private: // ** プライベートなメンバ変数 ** //
		// srvのポインタを保持
		SRV* srv_;

		
		// 書き込むデータの種類
		struct RenderData {
			RootSignature root;	// RootSignature
			PSO pso;	// PSO
		};

		// リキッドモデル
		RenderData rigid_;
		// スキニングモデル
		RenderData skinning_;
		// スタティックモデル
		RenderData static_;


	private: // ** プライベートなメンバ関数 ** //

		/// <summary>
		/// 全てのモデルをターゲットに描画する関数
		/// </summary>
		void DispatchAllModel(ID3D12GraphicsCommandList6* list, D3D12_GPU_VIRTUAL_ADDRESS cameraView);
	};
}