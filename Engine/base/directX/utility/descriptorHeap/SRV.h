#pragma once
#include "IDescriptorHeap.h"

// 前方宣言
namespace DirectX {
	class ScratchImage;
}

namespace LWP::Base {
	struct SRVInfo : public HeapInfo {
		D3D12_GPU_DESCRIPTOR_HANDLE gpuView{};
		D3D12_SHADER_RESOURCE_VIEW_DESC desc = D3D12_SHADER_RESOURCE_VIEW_DESC();


		// デフォルトコンストラクタ
		SRVInfo() = default;

		void SetView(IDescriptorHeap* heap) override {
			cpuView = heap->GetCPUHandle(index);
			gpuView = heap->GetGPUHandle(index);
		}
		
		// ムーブコンストラクタ
		SRVInfo(SRVInfo&& other) noexcept : HeapInfo(std::move(other)) {
			gpuView = std::exchange(other.gpuView, {});
			desc = std::exchange(other.desc, {});
		}

		// ムーブ代入演算子
		SRVInfo& operator=(SRVInfo&& other) noexcept {
			if (this != &other) {
				HeapInfo::operator=(std::move(other));
				// ムーブ代入の実装
				gpuView = std::exchange(other.gpuView, {});
				desc = std::exchange(other.desc, {});
			}
			return *this;
		}

		// コピー操作を禁止
		SRVInfo(const SRVInfo&) = delete;
		SRVInfo& operator=(const SRVInfo&) = delete;
	};

	/// <summary>
	/// ShaderResourceView
	/// </summary>
	class SRV : public IDescriptorHeap {
	public:	// ** メンバ関数 ** //

		/// <summary>
		/// デフォルトコンストラクタ
		/// </summary>
		SRV() = delete;
		/// <summary>
		/// コンストラクタ
		/// </summary>
		SRV(ID3D12Device* device);


		/// <summary>
		/// 初期化
		/// </summary>
		void Init();

		/// <summary>
		/// テクスチャの最初のViewを返す関数
		/// </summary>
		D3D12_GPU_DESCRIPTOR_HANDLE GetFirstTexView() { return GetGPUHandle(500); }

		/// <summary>
		/// TextureをSRVに登録
		/// </summary>
		SRVInfo CreateTexture(ID3D12Resource* resource, const DirectX::ScratchImage& mipImages);
		/// <summary>
		/// RenderResourceをSRVに登録
		/// </summary>
		SRVInfo CreateRenderResource(ID3D12Resource* resource, const int width, const int height);

		/// <summary>
		/// StructuredBufferをSRVに登録
		/// </summary>
		SRVInfo CreateStructuredBuffer(ID3D12Resource* resource, const D3D12_SHADER_RESOURCE_VIEW_DESC& desc);

		/// <summary>
		/// ShadowMapDirをSRVに登録
		/// </summary>
		SRVInfo CreateShadowMapDir(ID3D12Resource* resource);
		/// <summary>
		/// ShadowMapPointをSRVに登録
		/// </summary>
		SRVInfo CreateShadowMapPoint(ID3D12Resource* resource);
		
		//int UploadDepthMap(ID3D12Resource* resource);


		/// <summary>
		/// ImGui用のスペースを用意
		/// </summary>
		SRVInfo CreateImGuiSpace();

	private: // ** メンバ変数 ** //

		// StructerdBufferなどのデータ系は0 ~ 499
		// テクスチャ系などのデータは500 ~ 999に割り当てる（論理分割）

		// テクスチャ用のインデックス
		Utility::IndexManager texIndexManager_;


	private: // ** プライベートなメンバ関数 ** //

		//void UploadTextureResource(ID3D12Resource* resource, int index, D3D12_SHADER_RESOURCE_VIEW_DESC desc);
	};
}