#pragma once
#include "../../../Externals/DirectXTex/DirectXTex.h"
#include "../../../Engine/object/WorldTransform.h"
#include <string>

#include <d3d12.h>
#pragma comment(lib, "d3d12.lib")

namespace LWP::Base {
	class DirectXCommon;
}

namespace LWP::Resource {
	class Texture final {
	public:
		// ** 共通の変数 ** //

		// コンストラクタ
		Texture(Base::DirectXCommon* direcX, const std::string& filePath);
		// コピーコンストラクタ
		Texture(const Texture&) = default;
		// デストラクタ
		~Texture() = default;

		/// <summary>
		/// テクスチャを読み込む
		/// </summary>
		void Load(const std::string& filePath);

		int GetIndex() { return index; }
		D3D12_GPU_DESCRIPTOR_HANDLE GetHandleGPU() { return textureSRVHandleGPU; }

	private: // メンバ変数

		// ミップマップ付きテクスチャ
		DirectX::ScratchImage mipImages;
		// テクスチャのリソース
		ID3D12Resource* textureResource = nullptr;
		// テクスチャのハンドル
		D3D12_GPU_DESCRIPTOR_HANDLE textureSRVHandleGPU;

		// テクスチャの読み込みindex
		int index;
	};
}