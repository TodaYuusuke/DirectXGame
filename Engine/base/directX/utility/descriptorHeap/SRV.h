#pragma once
#include "IDescriptorHeap.h"

#include "../Command.h"
#include "Config.h"

// 前方宣言
namespace DirectX {
	class ScratchImage;
}

namespace LWP::Base {
	struct SRVInfo : public HeapInfo {
	public:
		D3D12_GPU_DESCRIPTOR_HANDLE gpuView{};
		D3D12_SHADER_RESOURCE_VIEW_DESC desc{};


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
	struct UAVInfo : public HeapInfo {
	public:
		D3D12_GPU_DESCRIPTOR_HANDLE gpuView{};
		D3D12_UNORDERED_ACCESS_VIEW_DESC desc{};


		// デフォルトコンストラクタ
		UAVInfo() = default;

		void SetView(IDescriptorHeap* heap) override {
			cpuView = heap->GetCPUHandle(index);
			gpuView = heap->GetGPUHandle(index);
		}

		// ムーブコンストラクタ
		UAVInfo(UAVInfo&& other) noexcept : HeapInfo(std::move(other)) {
			gpuView = std::exchange(other.gpuView, {});
			desc = std::exchange(other.desc, {});
		}

		// ムーブ代入演算子
		UAVInfo& operator=(UAVInfo&& other) noexcept {
			if (this != &other) {
				HeapInfo::operator=(std::move(other));
				// ムーブ代入の実装
				gpuView = std::exchange(other.gpuView, {});
				desc = std::exchange(other.desc, {});
			}
			return *this;
		}

		// コピー操作を禁止
		UAVInfo(const UAVInfo&) = delete;
		UAVInfo& operator=(const UAVInfo&) = delete;
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

		// コマンドクラスをセット
		void SetCommand(Command* cmd) { cmd_ = cmd; }

		/// <summary>
		/// ImGui
		/// </summary>
		void DebugGUI() override;

		/// <summary>
		/// テクスチャの最初のViewを返す関数
		/// </summary>
		D3D12_GPU_DESCRIPTOR_HANDLE GetFirstTexView();
		/// <summary>
		/// 平行光源のシャドウマップの最初のViewを返す関数
		/// </summary>
		D3D12_GPU_DESCRIPTOR_HANDLE GetFirstDirShadowView();
		/// <summary>
		/// 点光源のシャドウマップの最初のViewを返す関数
		/// </summary>
		D3D12_GPU_DESCRIPTOR_HANDLE GetFirstPointShadowView();

		/// <summary>
		/// TextureをSRVに登録
		/// </summary>
		SRVInfo CreateTexture(ID3D12Resource* resource, ID3D12Resource* intermediateResource, std::vector<D3D12_SUBRESOURCE_DATA>* subResources, const DirectX::ScratchImage& mipImages);
		/// <summary>
		/// DepthStencilをSRVに登録
		/// </summary>
		SRVInfo CreateDepthTexture(ID3D12Resource* resource);
		/// <summary>
		/// RenderResourceをSRVに登録
		/// </summary>
		SRVInfo CreateRenderResource(ID3D12Resource* resource);
		/// <summary>
		/// CubeMapをSRVに登録
		/// </summary>
		SRVInfo CreateCubeMap(ID3D12Resource* resource);

		/// <summary>
		/// StructuredBufferをSRVに登録
		/// </summary>
		SRVInfo CreateStructuredBuffer(ID3D12Resource* resource, const D3D12_SHADER_RESOURCE_VIEW_DESC& desc);
		/// <summary>
		/// RWStructuredBufferをSRVに登録
		/// </summary>
		UAVInfo CreateRWStructuredBuffer(ID3D12Resource* resource, const D3D12_UNORDERED_ACCESS_VIEW_DESC& desc);

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
		// コマンドのポインタ
		Command* cmd_ = nullptr;

		// インデックスの論理分割
		// StructerdBuffer当のデータ系 : 500枠
		// 基底クラスのindexManager_を使う
		// テクスチャ系のデータ : 500枚
		Utility::IndexManager textureIM_;
		// 平行光源のシャドウマップ : 1つ分
		Utility::IndexManager dirShadowMapIM_;
		// 点光源のシャドウマップ : 8つ分
		Utility::IndexManager pointShadowIM_;


	private: // ** プライベートなメンバ関数 ** //
	};
}