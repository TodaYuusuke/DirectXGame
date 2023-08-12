#pragma once
#include "../../../Externals/DirectXTex/DirectXTex.h"
#include <string>

#include <d3d12.h>
#pragma comment(lib, "d3d12.lib")

namespace LWP::Base {
	class DirectXCommon;
}

namespace LWP::Resource {
	class Texture final {
	public: // メンバ関数

		// コンストラクタ
		Texture(Base::DirectXCommon* direcX, const std::string& filePath);
		// デストラクタ
		~Texture() = default;

		/// <summary>
		/// テクスチャを読み込む
		/// </summary>
		void Load(const std::string& filePath);

		D3D12_GPU_DESCRIPTOR_HANDLE GetHandleGPU() { return textureSrvHandleGPU; }

	private: // メンバ変数

		// ミップマップ付きテクスチャ
		DirectX::ScratchImage mipImages;
		// テクスチャのリソース
		ID3D12Resource* textureResource = nullptr;
		// GPU上のアドレス
		D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU;
	};
}