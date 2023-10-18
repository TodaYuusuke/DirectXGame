#pragma once
#include "directX/GPUDevice.h"
#include "directX/descriptorHeap/RTV.h"
#include "directX/descriptorHeap/SRV.h"
#include "directX/descriptorHeap/DSV.h"
#include "directX/command/CommandManager.h"

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

		// アクセサ
		ID3D12Device* GetDevice() const { return device_; }
		CommandManager* GetCommandManager() const { return commandManager_.get(); }
		size_t GetBackBufferCount() const { return backBuffers_.size(); }
		// ImGui用
		UINT GetBufferCount() { return swapChainDesc_.BufferCount; }
		DXGI_FORMAT GetFormat() { return rtv_->GetDesc().Format; }
		ID3D12DescriptorHeap* GetSRVHeap() { return srv_->GetHeap(); }


	private: // ** メンバ変数 ** //
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

		// コマンド管理
		std::unique_ptr<CommandManager> commandManager_;

		// Direct3D関連
		Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain_;
		DXGI_SWAP_CHAIN_DESC1 swapChainDesc_;
		std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> backBuffers_;
		int32_t backBufferWidth_ = 0;
		int32_t backBufferHeight_ = 0;


	private: // 非公開のメンバ関数
		DirectXCommon(const DirectXCommon&) = delete;
		const DirectXCommon& operator=(const DirectXCommon&) = delete;

		/// <summary>
		/// スワップチェーンの生成
		/// </summary>
		void CreateSwapChain();

		/// <summary>
		/// レンダーターゲット生成
		/// </summary>
		void CreateFinalRenderTargets();
	};
}