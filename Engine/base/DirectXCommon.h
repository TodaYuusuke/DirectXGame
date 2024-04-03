#pragma once
#include "directX/utility/GPUDevice.h"
#include "directX/utility/HeapManager.h"
#include "directX/command/CommandManager.h"

#include "directX/utility/resource/rendering/BackBuffer.h"
#include "directX/utility/resource/rendering/DepthStencil.h"


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
		/// DrawCall
		/// </summary>
		void DrawCall();

		/// <summary>
		/// 描画語処理
		/// </summary>
		void PostDraw();

		// アクセサ
		ID3D12Device* GetDevice() const { return device_; }
		HeapManager* GetHeaps() const { return heaps_.get(); }
		CommandManager* GetCommandManager() const { return commandManager_.get(); }
		// ImGui用
		UINT GetBufferCount() { return swapChainDesc_.BufferCount; }
		DXGI_FORMAT GetFormat() { return heaps_->rtv()->GetDesc().Format; }
		ID3D12DescriptorHeap* GetSRVHeap() { return heaps_->srv()->GetHeap(); }


	private: // ** メンバ変数 ** //
		// ウィンドウズアプリケーション管理
		WinApp* winApp_ = nullptr;

		// デバイス
		std::unique_ptr<GPUDevice> gpuDevice_;
		// いちいちポインタを貰うのが面倒なので保持する
		ID3D12Device* device_ = nullptr;

		// RTV、SRV、DSVをまとめて管理するクラス
		std::unique_ptr<HeapManager> heaps_;

		// スワップチェーン
		Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain_;
		// Desc
		DXGI_SWAP_CHAIN_DESC1 swapChainDesc_;
		// バッファー配列
		std::vector<BackBuffer> backBuffers_;
		// バックバッファの深度ステンシル
		DepthStencil depthStencil_;

		// コマンド管理
		std::unique_ptr<CommandManager> commandManager_;

		// Direct3D関連
		int32_t backBufferWidth_ = 0;
		int32_t backBufferHeight_ = 0;


	private: // 非公開のメンバ関数
		DirectXCommon(const DirectXCommon&) = delete;
		const DirectXCommon& operator=(const DirectXCommon&) = delete;
	};
}