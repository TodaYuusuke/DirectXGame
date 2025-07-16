#pragma once
#include "IGraphicsPass.h"

#include <map>

namespace LWP {
	namespace Object {
		class Camera;
	}
}

namespace LWP::Base {
	/// <summary>
	/// 遅延ライテイングを行うパス
	/// </summary>
	class DeferredShading final
		: public IGraphicsPass {
	public: // ** メンバ関数 ** //

		/// <summary>
		/// デストラクタ
		/// </summary>
		DeferredShading() = default;
		/// <summary>
		/// デストラクタ
		/// </summary>
		~DeferredShading() override = default;

		/// <summary>
		/// 初期化
		/// </summary>
		void Init();
		/// <summary>
		/// 描画命令を積み込む
		/// </summary>
		void PushCommand(ID3D12GraphicsCommandList6* list);


	private: // ** プライベートなメンバ関数 ** //

		/// <summary>
		/// 共通で利用するレンダリング用のバッファーを設定する
		/// </summary>
		void SetBuffers(ID3D12GraphicsCommandList6* list);


	private: // ** メンバ変数 ** //

		// ルートシグネチャ
		RootSignature root_;
		// パイプラインステートオブジェクト
		PSO pso_;
	};
}