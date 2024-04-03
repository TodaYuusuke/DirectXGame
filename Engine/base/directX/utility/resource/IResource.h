#pragma once
#include "../GPUDevice.h"
#include "../HeapManager.h"

namespace LWP::Base {
	/// <summary>
	/// ID3D12Resourceを使いやすくするための基底クラス
	/// </summary>
	class IResource {
	protected: // ** メンバ変数 ** //

		// リソース本体
		Microsoft::WRL::ComPtr<ID3D12Resource> resource_ = nullptr;


	public: // ** オペレーターオーバーロード ** //
		
		// ID3D12Resource型への暗黙の変換演算子をオーバーロード
		operator ID3D12Resource* () {
			return resource_.Get();
		}
	};
}