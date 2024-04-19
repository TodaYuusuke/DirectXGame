#pragma once
#include "WinApp.h"

#include "directX/utility/GPUDevice.h"
#include "directX/utility/HeapManager.h"
#include "directX/RendererManager.h"

#include "directX/resource/rendering/BackBuffer.h"
#include "directX/resource/rendering/DepthStencil.h"


#include <memory>


namespace LWP::Object {
	class Camera;
}

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
		void Initialize(WinApp* winApp);

		/// <summary>
		/// メイン描画につかうカメラをセットする
		/// </summary>
		/// <param name="camera"></param>
		void SetMainCamera(Object::Camera* camera);

		/// <summary>
		/// DrawCall
		/// </summary>
		void DrawCall();

		// アクセサ
		GPUDevice* GetGPUDevice() const { return gpuDevice_.get(); }
		ID3D12Device* GetDevice() const { return device_; }
		HeapManager* GetHeaps() const { return heaps_.get(); }
		RendererManager* GetRendererManager() const { return renderer_.get(); }

		// ImGui用
		UINT GetBufferCount() { return swapChainDesc_.BufferCount; }
		DXGI_FORMAT GetFormat() { return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; }
		SRV* GetSRV() { return heaps_->srv(); }


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
		std::unique_ptr<RendererManager> renderer_;


	private: // 非公開のメンバ関数
		DirectXCommon(const DirectXCommon&) = delete;
		const DirectXCommon& operator=(const DirectXCommon&) = delete;
	};
}