#pragma once
#include "IRenderer.h"
#include "base/directX/resource/rendering/BackBuffer.h"
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

		// パーティクルレンダリング用のRootSignature
		RootSignature root_;
		// PSO
		PSO pso_;

		// ターゲット配列
		std::vector<Target> target_;
		
		// 描画に使うGPUパーティクル
		std::vector<Object::GPUParticle*> particles_;


	private: // ** プライベートなメンバ関数 ** //

		/// <summary>
		/// 全てのモデルをターゲットに描画する関数
		/// </summary>
		void DispatchAllParticle(ID3D12GraphicsCommandList6* list, Object::GPUParticle* p, D3D12_GPU_VIRTUAL_ADDRESS cameraView);
	};
}