#pragma once
#include "../GPUDevice.h"
#include "../HeapManager.h"

#include <d3d12.h>
#include <dxgi1_6.h>
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")


namespace LWP::Base {
	/// <summary>
	/// ID3D12Resourceを使いやすくするための基底クラス
	/// </summary>
	class IResource {
	public: // ** メンバ関数 ** //

		/// <summary>
		/// デストラクタ
		/// </summary>
		IResource() = default;
		/// <summary>
		/// デストラクタ
		/// </summary>
		virtual ~IResource() = default;


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