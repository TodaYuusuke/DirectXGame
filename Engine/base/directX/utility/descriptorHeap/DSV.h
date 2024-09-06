#pragma once
#include "IDescriptorHeap.h"

#include <array>

namespace LWP::Base {
	struct DSVInfo : public HeapInfo {
	public:
		D3D12_DEPTH_STENCIL_VIEW_DESC desc{};

		// デフォルトコンストラクタ
		DSVInfo() = default;
		// ムーブコンストラクタ
		DSVInfo(DSVInfo&& other) noexcept : HeapInfo(std::move(other)) {
			desc = std::exchange(other.desc, {});
		}

		// ムーブ代入演算子
		DSVInfo& operator=(DSVInfo&& other) noexcept {
			if (this != &other) {
				HeapInfo::operator=(std::move(other));
				// ムーブ代入の実装
				desc = std::exchange(other.desc, {});
			}
			return *this;
		}

		// コピー操作を禁止
		DSVInfo(const DSVInfo&) = delete;
		DSVInfo& operator=(const DSVInfo&) = delete;
	};

	/// <summary>
	/// RenderTargetView
	/// </summary>
	class DSV : public IDescriptorHeap {
	public:	// ** メンバ関数 ** //

		/// <summary>
		/// デフォルトコンストラクタ
		/// </summary>
		DSV() = delete;
		/// <summary>
		/// コンストラクタ
		/// </summary>
		DSV(ID3D12Device* device);

		/// <summary>
		/// 初期化
		/// </summary>
		void Init();
		/// <summary>
		/// ImGui
		/// </summary>
		void DebugGUI() override;

		
		/// <summary>
		/// デプスステンシルビューを作成
		/// </summary>
		DSVInfo CreateDepthStencilView(ID3D12Resource* resource);
		/// <summary>
		/// デプスキューブマップを作成
		/// </summary>
		std::array<DSVInfo, 6> CreateDepthStencilCubeMap(ID3D12Resource* resource);

		/// <summary>
		/// 平行光源シャドウマップ用のリソースを作成
		/// </summary>
		DSVInfo CreateShadowMapDir(ID3D12Resource* resource);
		/// <summary>
		/// 点光源シャドウマップ用のリソースを作成
		/// </summary>
		std::array<DSVInfo, 6> CreateShadowMapPoint(ID3D12Resource* resource);
	};
}
