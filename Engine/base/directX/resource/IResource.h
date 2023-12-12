#pragma once
#include "../GPUDevice.h"
#include "utility/Counter.h"

namespace LWP::Base {
	/// <summary>
	/// ID3D12リソース汎用クラス
	/// </summary>
	class IResource {
	public: // ** メンバ関数 ** //

		IResource() = default;
		virtual ~IResource() = default;

		// 使用数をリセット
		void Reset() { usedCount_.Reset(); }
		// 使用済みのカウントを返す関数
		int GetCount() { return usedCount_.Get(); }

	protected: // ** メンバ変数 ** //
		
		// リソース
		Microsoft::WRL::ComPtr<ID3D12Resource> resource_;

		// 使用済みカウント
		LWP::Utility::Counter usedCount_;
	};
}