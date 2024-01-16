#pragma once
#include "IDescriptorHeap.h"

#include <vector>

// 前方宣言
namespace DirectX {
	class ScratchImage;
}

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
		/// Viewを返す関数
		/// </summary>
		D3D12_GPU_DESCRIPTOR_HANDLE GetView() { return texView_; }

		/// <summary>
		/// ShaderResourceViewを作成
		/// </summary>
		int CreateShaderResourceView(ID3D12Resource* resource, const DirectX::ScratchImage& mipImages);
		/// <summary>
		/// ShaderResourceViewを作成
		/// </summary>
		int CreateShaderResourceView(ID3D12Resource* resource, const int width, const int height);


	private: // ** メンバ変数 ** //

		// テクスチャの一番最初のview
		D3D12_GPU_DESCRIPTOR_HANDLE texView_;
		// テクスチャを読み込んだ数
		int loadedTextureCount = 0;

	private: // ** プライベートなメンバ関数 ** //

		void UploadTextureResource(ID3D12Resource* resource, D3D12_SHADER_RESOURCE_VIEW_DESC desc);
	};
}