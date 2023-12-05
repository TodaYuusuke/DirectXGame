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


	private: // ** メンバ変数 ** //
	};
}