#pragma once
#include "math/vector/Vector2.h"
#include "utility/Counter.h"
#include "resources/texture/RenderTexture.h"
#include "base/directX/utility/RootSignature.h"
#include "base/directX/utility/PSO.h"
#include "base/directX/utility/HeapManager.h"
#include "Config.h"

/// <summary>
/// 歪曲収差パラメータ
/// </summary>
struct LensDistortion {
	// アクティブフラグ
	bool isActive = false;

	// 強度（-100 ~ 100）
	int intensity = 0;
	// 影響度合い（0.0f ~ 1.0f）
	LWP::Math::Vector2 multiplier = { 1.0f,1.0f };
	// 歪みの中心（アンカーポイント）
	LWP::Math::Vector2 center = { 0.5f,0.5f };
	// レンズの拡大率（0.01f ~ 5.0f）
	float scale = 1.0f;
};


namespace LWP::Base::PostProcess {
	// リソース用のisActive抜きの構造体
	struct LensStruct {
		// 影響度合い（0.0f ~ 1.0f）
		LWP::Math::Vector2 multiplier;
		// 歪みの中心（アンカーポイント）
		LWP::Math::Vector2 center;
		// レンズの拡大率（0.01f ~ 5.0f）
		float scale;
		// 強度（-100 ~ 100）
		int intensity;

		LensStruct& operator=(const LensDistortion& value) {
			multiplier = value.multiplier;
			center = value.center;
			scale = value.scale;
			intensity = value.intensity;
			return *this;
		}
	};

	/// <summary>
	/// 歪曲収差レンダリングコマンド
	/// </summary>
	class LensDistortionRenderer {
	public: // ** メンバ関数 ** //

		// コンストラクタ
		LensDistortionRenderer() = default;
		// デストラクタ
		~LensDistortionRenderer() = default;

		// 初期化処理
		void Init(ID3D12Device* device, DXC* dxc, HeapManager* heaps);
		// レンダリング
		void Draw(ID3D12GraphicsCommandList* list);
		// データをリセット
		void Reset();

		// レンダリングするためのデータをセットする関数
		void SetRenderData(Resource::RenderTexture* target, LensDistortion data);


	private: // ** メンバ変数 ** //

		// レンダリングするためのデータ
		struct RenderData {
			Resource::RenderTexture* target;	// レンダリングを行う先のリソース
			LensStruct* data;	// レンダリングに使うデータ
			ID3D12Resource* resource;	// データのリソース
			D3D12_GPU_VIRTUAL_ADDRESS view;	// データのビュー
		};
		std::unique_ptr<RenderData> renderData_[lwpC::PostProcess::kMaxLensDistortion];

		// ルートシグネチャ
		std::unique_ptr<RootSignature> root_;
		// PSO
		std::unique_ptr<PSO> pso_;
		// ディスクリプタヒープ管理クラスのポインタ
		HeapManager* heaps_ = nullptr;

		// 使用数
		Utility::Counter counter_;


	private: // ** メンバ変数 ** //

		void PreDraw(ID3D12GraphicsCommandList* list, int index);
		void PostDraw(ID3D12GraphicsCommandList* list, int index);
	};
}