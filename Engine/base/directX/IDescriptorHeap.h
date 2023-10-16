#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#include <wrl.h>

#include "../WinApp.h"

namespace LWP::Base {
	/// <summary>
	/// ディスクリプタヒープ
	/// </summary>
	class IDescriptorHeap {
	public:
		// ** メンバ関数 ** //

	private: // ** メンバ変数 ** //

		// Heap
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> heap_;
	};
}