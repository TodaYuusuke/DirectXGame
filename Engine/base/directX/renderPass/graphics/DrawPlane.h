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
	/// VertexShaderでPlaneオブジェクトを描画
	/// </summary>
	class DrawPlane final
		: public IGraphicsPass {
	public: // ** メンバ関数 ** //

		/// <summary>
		/// デストラクタ
		/// </summary>
		DrawPlane() = default;
		/// <summary>
		/// デストラクタ
		/// </summary>
		~DrawPlane() override = default;

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
		/// <summary>
		/// 実際の描画命令を積み込む
		/// </summary>
		void SetDrawCall(ID3D12GraphicsCommandList6* list);


		/// <summary>
		/// スプライトの描画
		/// </summary>
		void DrawSprite(ID3D12GraphicsCommandList6* list);


	private: // ** メンバ変数 ** //

		RootSignature root_;	// ルートシグネチャは共通で使う

		// 各種描画用のPSO
		struct PSOs {
			PSO sprite;			// スプライトの描画
			PSO billboard2D;	// ビルボード2Dの描画
			PSO billboard3D;	// ビルボード3Dの描画
		}psos_;
	};
}