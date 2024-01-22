#pragma once
#include "math/vector/Vector2.h"
#include "utility/Counter.h"
#include "base/directX/resource/RenderResource.h"
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

/// <summary>
/// 歪曲収差レンダリングコマンド
/// </summary>
namespace LWP::Base::PostProcess {
class LensDistortionRenderer {
public: // ** メンバ関数 ** //

	// コンストラクタ
	LensDistortionRenderer() = default;
	// デストラクタ
	~LensDistortionRenderer() = default;

	// 初期化処理
	void Init();
	// レンダリングするためのデータをセットする関数
	void SetRenderData(RenderResource* target, LensDistortion data);
	// データをクリア
	void Clear();

private: // ** メンバ変数 ** //

	// レンダリングするためのデータ
	struct RenderData {
		RenderResource* target[lwpC::PostProcess::kMaxLensDistortion];	// レンダリングを行う先のリソース
		LensDistortion* data;	// レンダリングに使うデータ
		ID3D12Resource* resource;	// リソース
	}renderData_;

	// 使用数
	Utility::Counter counter_;
};
}