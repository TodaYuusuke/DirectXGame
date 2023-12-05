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


	protected: // ** メンバ変数 ** //
		
		// リソース
		Microsoft::WRL::ComPtr<ID3D12Resource> resource_;

		// 使用済みカウント
		LWP::Utility::Counter usedCount_;
	};
}