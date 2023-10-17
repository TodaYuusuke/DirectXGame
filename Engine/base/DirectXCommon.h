#pragma once
#include "directX/GPUDevice.h"
#include "directX/descriptorHeap/RTV.h"
#include "directX/descriptorHeap/SRV.h"
#include "directX/descriptorHeap/DSV.h"

#include <memory>
#include <string>

#include "../../Externals/DirectXTex/DirectXTex.h"

namespace LWP::Base {
	/// <summary>
	/// DirectX管理クラス
	/// </summary>
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
		/// DescriptorHandleを取得する関数
		/// </summary>
		/// <param name="index"></param>
		/// <returns></returns>
		D3D12_CPU_DESCRIPTOR_HANDLE GetSRVCPUHandle(uint32_t index) { return srv_->GetCPUHandle(index); }
		D3D12_GPU_DESCRIPTOR_HANDLE GetSRVGPUHandle(uint32_t index) { return srv_->GetGPUHandle(index); }


		// アクセサ
		ID3D12Device* GetDevice() const { return device_; }
		ID3D12GraphicsCommandList* GetCommandList() const { return commandList_.Get(); }
		size_t GetBackBufferCount() const { return backBuffers_.size(); }
		// ImGui用
		UINT GetBufferCount() { return swapChainDesc_.BufferCount; }
		DXGI_FORMAT GetFormat() { return rtv_->GetDesc().Format; }
		ID3D12DescriptorHeap* GetSRVHeap() { return srv_->GetHeap(); }

	private: // メンバ変数
		// ウィンドウズアプリケーション管理
		WinApp* winApp_ = nullptr;

		// デバイス
		std::unique_ptr<GPUDevice> gpuDevice_;
		// いちいちポインタを貰うのが面倒なので保持する
		ID3D12Device* device_ = nullptr;

		// RTV
		std::unique_ptr<RTV> rtv_;
		// SRV
		std::unique_ptr<SRV> srv_;
		// DSV
		std::unique_ptr<DSV> dsv_;

		// Direct3D関連
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList_;
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocateor_;
		Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue_;
		DXGI_SWAP_CHAIN_DESC1 swapChainDesc_;
		Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain_;
		std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> backBuffers_;
		Microsoft::WRL::ComPtr<ID3D12Fence> fence_;
		UINT64 fenceVal_ = 0;
		int32_t backBufferWidth_ = 0;
		int32_t backBufferHeight_ = 0;


	private: // 非公開のメンバ関数
		DirectXCommon(const DirectXCommon&) = delete;
		const DirectXCommon& operator=(const DirectXCommon&) = delete;

		/// <summary>
		/// コマンド関連初期化
		/// </summary>
		void InitializeCommand();

		/// <summary>
		/// スワップチェーンの生成
		/// </summary>
		void CreateSwapChain();

		/// <summary>
		/// レンダーターゲット生成
		/// </summary>
		void CreateFinalRenderTargets();

		/// <summary>
		/// フェンス生成
		/// </summary>
		void CreateFence();


		ID3D12DescriptorHeap* CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible);

		/// <summary>
		/// リソースバリアの実態を作る関数
		/// </summary>
		D3D12_RESOURCE_BARRIER MakeResourceBarrier(ID3D12Resource*, D3D12_RESOURCE_STATES, D3D12_RESOURCE_STATES);
	};
}