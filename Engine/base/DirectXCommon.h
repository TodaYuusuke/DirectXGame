#pragma once
#include <string>
#include <vector>
#include <d3d12.h>
#include <dxgi1_6.h>
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

#include <wrl.h>
#include "WinApp.h"
#include "../../Externals/DirectXTex/DirectXTex.h"

namespace LWP::Base {
	class DirectXCommon
	{
	public: // メンバ関数

		// コンストラクタ
		DirectXCommon() = default;
		// デストラクタ
		~DirectXCommon() = default;


		/// <summary>
		/// 初期化
		/// </summary>
		void Initialize(WinApp* winApp, int32_t backBufferWidth, int32_t backBufferHeight);

		/// <summary>
		/// 描画前処理
		/// </summary>
		void PreDraw();

		/// <summary>
		/// 描画語処理
		/// </summary>
		void PostDraw();

		/// <summary>
		/// レンダーターゲットのクリア
		/// </summary>
		void ClearRenderTarget();


		/// <summary>
		/// テクスチャのリソースを作る
		/// </summary>
		/// <param name="metdata"></param>
		ID3D12Resource* CreateTextureResource(const DirectX::TexMetadata& metdata);

		/// <summary>
		/// テクスチャを転送する
		/// </summary>
		/// <param name="texture"></param>
		/// <param name="mipImages"></param>
		void UploadTextureData(ID3D12Resource* texture, D3D12_GPU_DESCRIPTOR_HANDLE* textureSrvHandleGPU, const DirectX::ScratchImage& mipImages);


		// アクセサ
		ID3D12Device* GetDevice() const { return device_.Get(); }
		ID3D12GraphicsCommandList* GetCommandList() const { return commandList_.Get(); }
		size_t GetBackBufferCount() const { return backBuffers_.size(); }
		// ImGui用
		UINT GetBufferCount() { return swapChainDesc_.BufferCount; }
		DXGI_FORMAT GetFormat() { return rtvDesc_.Format; }
		ID3D12DescriptorHeap* GetSRVHeap() { return srvHeap_.Get(); }

	private: // メンバ変数
		// ウィンドウズアプリケーション管理
		WinApp* winApp_ = nullptr;

		// Direct3D関連
		Microsoft::WRL::ComPtr<IDXGIFactory7> dxgiFactory_;
		Microsoft::WRL::ComPtr<ID3D12Device> device_;
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList_;
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocateor_;
		Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue_;
		DXGI_SWAP_CHAIN_DESC1 swapChainDesc_;
		Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain_;
		std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> backBuffers_;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvHeap_;
		D3D12_RENDER_TARGET_VIEW_DESC rtvDesc_;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvHeap_;
		Microsoft::WRL::ComPtr<ID3D12Fence> fence_;
		UINT64 fenceVal_ = 0;
		int32_t backBufferWidth_ = 0;
		int32_t backBufferHeight_ = 0;


	private: // 非公開のメンバ関数
		DirectXCommon(const DirectXCommon&) = delete;
		const DirectXCommon& operator=(const DirectXCommon&) = delete;

		/// <summary>
		/// DXGIデバイス初期化
		/// </summary>
		void InitializeDXGIDevice();

		/// <summary>
		/// コマンド関連初期化
		/// </summary>
		void InitializeCommand();

		/// <summary>
		/// スワップチェーンの生成
		/// </summary>
		void CreateSwapChain();

		/// <summary>
		/// ディスクリプタヒープを生成
		/// </summary>
		/// <param name="heapType"></param>
		/// <param name="numDescriptors"></param>
		/// <param name="shaderVisible"></param>
		/// <returns></returns>
		ID3D12DescriptorHeap* CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible);

		/// <summary>
		/// レンダーターゲット生成
		/// </summary>
		void CreateFinalRenderTargets();

		/// <summary>
		/// フェンス生成
		/// </summary>
		void CreateFence();

		/// <summary>
		///　デスクリプタヒープのデスクリプタハンドル増分サイズを返す関数
		/// </summary>
		D3D12_CPU_DESCRIPTOR_HANDLE GetDescriptorHandleIncrementSize(const D3D12_CPU_DESCRIPTOR_HANDLE&, int, UINT);

		/// <summary>
		/// リソースバリアの実態を作る関数
		/// </summary>
		D3D12_RESOURCE_BARRIER MakeResourceBarrier(ID3D12Resource*, D3D12_RESOURCE_STATES, D3D12_RESOURCE_STATES);
	};
}