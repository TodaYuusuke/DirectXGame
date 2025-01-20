#pragma once
#include "IDescriptorHeap.h"

#include <array>

namespace LWP::Base {
	struct RTVInfo : public HeapInfo {
	public:
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
	class RTV : public IDescriptorHeap, public Utility::ISingleton<RTV> {
		friend class Utility::ISingleton<RTV>;	// ISingletonをフレンドクラスにしてコンストラクタを呼び出せるように
	private: // ** シングルトン化に必要な処理 ** //

		/// <summary>
		/// コンストラクタをプライベートに
		/// </summary>
		RTV();

	public: // ** メンバ関数 ** //

		/// <summary>
		/// ImGui
		/// </summary>
		void DebugGUI() override;

		/// <summary>
		/// RenderTargetViewを作成する関数
		/// </summary>
		RTVInfo CreateRenderTargetView(ID3D12Resource* resource);

		/// <summary>
		/// CubeMap用のViewを作成する関数
		/// </summary>
		std::array<RTVInfo, 6> CreateCubeMapView(ID3D12Resource* resource);
	};
}