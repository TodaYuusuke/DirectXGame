#pragma once
#include <memory>
#include <d3d12.h>
#include <dxgi1_6.h>
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#include <wrl.h>

namespace LWP::Primitive {
	class Manager;
}

/// <summary>
/// マテリアル
/// </summary>
namespace LWP::Resource {
	// マテリアルの情報の構造体
	struct MaterialStruct {
		int32_t enableLighting = true;
	};


	class Material final {
	public:
		// マテリアルデータ
		MaterialStruct* data_;

		Material(Primitive::Manager* primitiveManager);
		~Material() = default;

		// GPUアドレスを取得
		D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress() { return resource_.Get()->GetGPUVirtualAddress(); }

	private: // メンバ変数

		// マテリアル用のリソース
		Microsoft::WRL::ComPtr<ID3D12Resource> resource_;
	};
}