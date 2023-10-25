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
		/// DrawCall
		/// </summary>
		void DrawCall();

		/// <summary>
		/// 描画語処理
		/// </summary>
		void PostDraw();

		// アクセサ
		ID3D12Device* GetDevice() const { return device_; }
		CommandManager* GetCommandManager() const { return commandManager_.get(); }
		// ImGui用
		UINT GetBufferCount() { return rtv_->GetSwapChainDesc().BufferCount; }
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
		int32_t backBufferWidth_ = 0;
		int32_t backBufferHeight_ = 0;


	private: // 非公開のメンバ関数
		DirectXCommon(const DirectXCommon&) = delete;
		const DirectXCommon& operator=(const DirectXCommon&) = delete;
	};
}