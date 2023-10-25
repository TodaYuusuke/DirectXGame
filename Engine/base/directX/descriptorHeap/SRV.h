#pragma once
#include "IDescriptorHeap.h"

#include <vector>

namespace LWP::Base {
	/// <summary>
	/// RenderTargetView
	/// </summary>
	class SRV : public IDescriptorHeap {
	public:
		// ** メンバ関数 ** //

		/// <summary>
		/// 初期化
		/// </summary>
		void Initialize(ID3D12Device* device);

		/// <summary>
		/// 使用数を+1増加
		/// </summary>
		void AddUsedCount() { usedCount_++; }
		/// <summary>
		/// 現在の使用数を取得
		/// </summary>
		UINT GetUsedCount() { return usedCount_; }


	private: // ** メンバ変数 ** //

		// 0番目はImGuiが使っているので初期値1
		UINT usedCount_ = 1;
	};
}