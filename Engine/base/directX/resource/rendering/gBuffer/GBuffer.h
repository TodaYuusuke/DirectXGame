#pragma once
#include "target/Albedo.h"
#include "target/Normal.h"
#include "target/PBR.h"
#include "target/WorldPosition.h"
#include "../DepthStencil.h"

namespace LWP::Base {
	/// <summary>
	/// 遅延ライティングのGBuffer
	/// </summary>
	class GBuffer {
	public: // ** メンバ関数 ** //

		// 初期化
		void Init();
		// 画面クリア
		void Clear(ID3D12GraphicsCommandList* list);

		/// <summary>
		/// GBufferの全リソースを描画用に準備する関数
		/// <para>１．レンダーターゲットに指定</para>
		/// <para>２．リソースのバリアを設定</para>
		/// <para>３．ビューポートとシザー矩形を設定</para>
		/// </summary>
		void SetRenderTarget(ID3D12GraphicsCommandList* list);
		/// <summary>
		/// GBufferの全リソースをライテイング用に準備する関数
		/// <para>１．レンダーターゲットに指定</para>
		/// <para>２．リソースのバリアを設定</para>
		/// <para>３．ビューポートとシザー矩形を設定</para>
		/// </summary>
		void SetShaderResource(ID3D12GraphicsCommandList* list);

		/// <summary>
		/// 全リソースのバリアを1つ前の状態に戻す関数
		/// </summary>
		/// <param name="list"></param>
		void RevertResourceBarrier(ID3D12GraphicsCommandList* list);

	public: // ** パブリックなメンバ変数 ** //
		
		// アルベド
		GBufferTarget::Albedo albedo;
		// 法線
		GBufferTarget::Normal normal;
		// マテリアル
		GBufferTarget::PBR pbr;
		// ワールド座標
		GBufferTarget::WorldPosition worldPosition;
		// 深度
		DepthStencil depthStencil;

	};
}