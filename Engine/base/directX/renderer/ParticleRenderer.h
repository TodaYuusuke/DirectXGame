#pragma once
#include "IRenderer.h"
#include "base/directX/resource/rendering/RenderResource.h"
#include "base/directX/resource/rendering/DepthStencil.h"

#include "object/core/GPUParticle.h"

namespace LWP::Base {
	/// <summary>
	/// パーティクルのレンダラー
	/// </summary>
	class ParticleRenderer : public IRenderer {
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
		ParticleRenderer() = default;
		/// <summary>
		/// デフォルトデストラクタ
		/// </summary>
		~ParticleRenderer() = default;

		/// <summary>
		/// 初期化
		/// </summary>
		void Init(Command* cmd, std::function<void()> func);

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
		/// GPUパーティクルクラスをセット
		/// </summary>
		/// <param name="target"></param>
		void AddParticle(Object::GPUParticle* p) { particles_.push_back(p); }

		/// <summary>
		/// リセット
		/// </summary>
		void Reset();


	private: // ** プライベートなメンバ変数 ** //
		// Commandのポインタを保持
		Command* cmd_;

		struct Property {
			RootSignature root;
			PSO pso;
		};

		
		Property initShader_;	// 初期化用
		Property initHitShader_;	// 値判定の初期化用
		Property renderingShader_;	// 描画用

		// ターゲット配列
		std::vector<Target> target_;
		// 描画に使うGPUパーティクル
		std::vector<Object::GPUParticle*> particles_;

		// ** 当たり判定用のリソース ** //

		struct Collider {
			// ID書き込み用リソース
			RenderResource id;
			// 検証用深度
			DepthStencil depthStencil;

			// 辺の描画用PSO
			PSO wireFrame;
			// StaticModelのRootSignature
			RootSignature frontFaceRoot;
			PSO frontFacePSO;	// StaticModelの表面描画用
			PSO backFacePSO;	// StaticModelの裏面描画用

			// ステンシルのみをUAV用のリソースにコピーするためのPSO
			RootSignature checkResultRoot;
			PSO checkResultPSO;
		}collider_;

	private: // ** プライベートなメンバ関数 ** //

		/// <summary>
		/// パーティクルの当たり判定を検証するための関数
		/// </summary>
		void CheckCollision(ID3D12GraphicsCommandList6* list, Object::GPUParticle* p, D3D12_GPU_VIRTUAL_ADDRESS cameraView);

		/// <summary>
		/// 全てのパーティクルをターゲットに描画する関数
		/// </summary>
		void DispatchAllParticle(ID3D12GraphicsCommandList6* list, Object::GPUParticle* p, D3D12_GPU_VIRTUAL_ADDRESS cameraView);

		/// <summary>
		/// 全てのモデルをターゲットに描画する関数
		/// </summary>
		void DispatchAllModel(ID3D12GraphicsCommandList6* list, D3D12_GPU_VIRTUAL_ADDRESS cameraView);
	};
}