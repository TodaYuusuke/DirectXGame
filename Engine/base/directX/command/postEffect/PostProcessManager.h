#pragma once
#include "LensDistortion.h"
#include "CCTVEffect.h"

namespace LWP::Base::PostProcess {
	/// <summary>
	/// ポストプロセスを管理するクラス
	/// </summary>
	class Manager {
	public: // ** メンバ関数 ** //
		
		// 初期化処理
		void Init(ID3D12Device* device, DXC* dxc, HeapManager* heaps);
		// レンダリング
		void Draw(ID3D12GraphicsCommandList* list);
		// データをクリア
		void Reset();

		// レンダリングするためのデータをセットする関数
		void SetRenderData(Resource::RenderTexture* target, CCTVEffect data);
		void SetRenderData(Resource::RenderTexture* target, LensDistortion data);

	private: // ** メンバ変数 ** //

		// 監視カメラ
		std::unique_ptr<CCTVEffectRenderer> cctvEffect_;
		// 歪曲収差
		std::unique_ptr<LensDistortionRenderer> lensDistortion_;
	};
}