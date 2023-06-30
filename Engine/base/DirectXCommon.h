
#pragma once
#include <string>
#include <vector>
#include <d3d12.h>
#include <dxgi1_6.h>
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

#include <wrl.h>
#include "WinApp.h"

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

	// アクセサ
	ID3D12Device* GetDevice() const { return device_.Get(); }
	ID3D12GraphicsCommandList* GetCommandList() const { return commandList_.Get(); }
	size_t GetBackBufferCount() const { return backBuffers_.size(); }

private: // メンバ変数
	// ウィンドウズアプリケーション管理
	WinApp* winApp_ = nullptr;

	// Direct3D関連
	Microsoft::WRL::ComPtr<IDXGIFactory7> dxgiFactory_;
	Microsoft::WRL::ComPtr<ID3D12Device> device_;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList_;
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocateor_;
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue_;
	Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain_;
	std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> backBuffers_;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvHeap_;
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