#pragma once
#include "math/vector/Vector2.h"
#include "utility/Counter.h"
#include "resources/texture/RenderTexture.h"
#include "base/directX/command/RootSignature.h"
#include "base/directX/command/PSO.h"
#include "base/directX/descriptorHeap/HeapManager.h"
#include "Config.h"

/// <summary>
/// 監視カメラ風演出のパラメータ
/// </summary>
struct CCTVEffect {
	// アクティブフラグ
	bool isActive = false;

	// 時間
	int time = 0;
};


namespace LWP::Base::PostProcess {
	// リソース用のisActive抜きの構造体
	struct CCTVStruct {
		int time;

		CCTVStruct& operator=(const CCTVEffect& value) {
			time = value.time;
			return *this;
		}
	};

	/// <summary>
	/// 監視カメラエフェクトレンダリングコマンド
	/// </summary>
	class CCTVEffectRenderer {
	public: // ** メンバ関数 ** //

		// コンストラクタ
		CCTVEffectRenderer() = default;
		// デストラクタ
		~CCTVEffectRenderer() = default;

		// 初期化処理
		void Init(ID3D12Device* device, DXC* dxc, HeapManager* heaps);
		// レンダリング
		void Draw(ID3D12GraphicsCommandList* list);
		// データをリセット
		void Reset();

		// レンダリングするためのデータをセットする関数
		void SetRenderData(Resource::RenderTexture* target, CCTVEffect data);


	private: // ** メンバ変数 ** //

		// レンダリングするためのデータ
		struct RenderData {
			Resource::RenderTexture* target;	// レンダリングを行う先のリソース
			CCTVStruct* data;	// レンダリングに使うデータ
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