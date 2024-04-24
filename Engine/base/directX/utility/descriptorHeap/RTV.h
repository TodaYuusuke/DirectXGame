#pragma once
#include "IDescriptorHeap.h"

namespace LWP::Base {
	struct RTVInfo : public HeapInfo {
		D3D12_RENDER_TARGET_VIEW_DESC desc{};

		// デフォルトコンストラクタ
		RTVInfo() = default;
		// ムーブコンストラクタ
		RTVInfo(RTVInfo&& other) noexcept : HeapInfo(std::move(other)) {
			desc = std::exchange(other.desc, {});
		}

		// ムーブ代入演算子
		RTVInfo& operator=(RTVInfo&& other) noexcept {
			if (this != &other) {
				HeapInfo::operator=(std::move(other));
				// ムーブ代入の実装
				desc = std::exchange(other.desc, {});
			}
			return *this;
		}

		// コピー操作を禁止
		RTVInfo(const RTVInfo&) = delete;
		RTVInfo& operator=(const RTVInfo&) = delete;
	};


	/// <summary>
	/// RenderTargetView
	/// </summary>
	class RTV : public IDescriptorHeap {
	public: // ** メンバ関数 ** //
		
		/// <summary>
		/// デフォルトコンストラクタ
		/// </summary>
		RTV() = delete;
		/// <summary>
		/// コンストラクタ
		/// </summary>
		RTV(ID3D12Device* device);

		/// <summary>
		/// 初期化
		/// </summary>
		void Init();

		/// <summary>
		/// RenderTargetViewを作成する関数
		/// </summary>
		RTVInfo CreateRenderTargetView(ID3D12Resource* resource);
	};
}