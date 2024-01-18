#pragma once
#include "../ICommand.h"
#include "resources/texture/RenderTexture.h"

namespace LWP::Base {
	/// <summary>
	/// サブ画面描画コマンド
	/// </summary>
	class SubRendering : public ICommand {
	public: // ** メンバ関数 ** //

		/// <summary>
		/// レンダリング先のデータをセット
		/// </summary>
		void SetDrawTarget(const Math::Matrix4x4& vp, Resource::RenderTexture* renderTexture);

		/// <summary>
		/// 描画前処理
		/// </summary>
		void PreDraw(ID3D12GraphicsCommandList* list) override;
		
		/// <summary>
		/// 描画処理を少しだけ書き換える
		/// </summary>
		/// <param name="rootSignature"></param>
		/// <param name="list"></param>
		/// <param name="viewStruct"></param>
		void Draw(ID3D12RootSignature* rootSignature, ID3D12GraphicsCommandList* list, ViewStruct viewStruct) override;


		/// <summary>
		/// 描画語処理
		/// </summary>
		void PostDraw(ID3D12GraphicsCommandList* list) override;


	private: // ** メンバ変数 ** // 

		// 書き込み先のRenderTexture
		Resource::RenderTexture* renderTexture_;

		// mainCommandのindexDataを貰う
		IStructured<IndexInfoStruct>* indexPtr_;

	private: // ** プライベートな関数 ** //

		/// <summary>
		/// PSOを作成
		/// </summary>
		void CreatePSO(ID3D12Device* device, DXC* dxc, ID3D12RootSignature* rootSignature) override;
	};
}