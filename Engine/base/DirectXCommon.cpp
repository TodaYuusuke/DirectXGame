#include "DirectXCommon.h"
#include "../utility/MyUtility.h"

#include <cassert>
#include <format>

using namespace Microsoft::WRL;
using namespace LWP::Base;
using namespace LWP::Utility;

void DirectXCommon::Initialize(WinApp* winApp, int32_t backBufferWidth, int32_t backBufferHeight) {

	winApp_ = winApp;
	backBufferWidth_ = backBufferWidth;
	backBufferHeight_ = backBufferHeight;

	// デバイス初期化
	gpuDevice_ = std::make_unique<GPUDevice>();
	gpuDevice_->Initialize();
	device_ = gpuDevice_->GetDevice();

	// コマンド管理初期化
	commandManager_ = std::make_unique<CommandManager>();
	commandManager_->Initialize(gpuDevice_->GetDevice());

	// スワップチェーンの生成
	CreateSwapChain();

	// レンダーターゲット生成
	CreateFinalRenderTargets();

	// DSV（深度バッファ）作成
	dsv_ = std::make_unique<DSV>();
	dsv_->Initialize(gpuDevice_->GetDevice(), backBufferWidth_, backBufferHeight_);

	// ディスクリプタヒープを登録
	commandManager_->SetDescriptorHeap(rtv_.get(), dsv_.get(), srv_.get());
}

void DirectXCommon::PreDraw() {
	// これから書き込むバックバッファのインデックスを取得
	UINT backBufferIndex = swapChain_->GetCurrentBackBufferIndex();
	commandManager_->PreDraw(backBufferIndex, backBuffers_[backBufferIndex].Get());
}

void DirectXCommon::PostDraw() {
	// これから書き込むバックバッファのインデックスを取得
	UINT backBufferIndex = swapChain_->GetCurrentBackBufferIndex();
	commandManager_->PostDraw(backBuffers_[backBufferIndex].Get(), swapChain_.Get());

	// 描画カウントリセット
	commandManager_->Reset();
}

void DirectXCommon::CreateSwapChain() {
	HRESULT hr = S_FALSE;

	// スワップチェーンを生成する
	swapChainDesc_.Width = winApp_->GetClientWidth();				// 画面の幅。ウィンドウのクライアント領域を同じものにしておく
	swapChainDesc_.Height = winApp_->GetClientHeight();				// 画面の高さ。ウィンドウのクライアント領域を同じものにしておく 
	swapChainDesc_.Format = DXGI_FORMAT_R8G8B8A8_UNORM;				// 色の形式
	swapChainDesc_.SampleDesc.Count = 1;							// マルチサンプルしない
	swapChainDesc_.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;	// 描画のターゲットとして利用する
	swapChainDesc_.BufferCount = 2;									// ダブルバッファ
	swapChainDesc_.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;		// モニタにうつしたら、中身を廃棄
	// コマンドキュー、ウィンドウハンドル、設定を渡して生成する
	ComPtr<IDXGISwapChain1> swapChain1;
	hr = gpuDevice_->GetFactory()->CreateSwapChainForHwnd(commandManager_->GetQueue(), winApp_->GetHWND(), &swapChainDesc_, nullptr, nullptr, &swapChain1);
	assert(SUCCEEDED(hr));

	// SwapChain4を得る
	swapChain1->QueryInterface(IID_PPV_ARGS(&swapChain_));
}

void DirectXCommon::CreateFinalRenderTargets() {
	HRESULT hr = S_FALSE;

	// RTV作成
	rtv_ = std::make_unique<RTV>();
	rtv_->Initialize(gpuDevice_->GetDevice());

	// SRV作成
	srv_ = std::make_unique<SRV>();
	srv_->Initialize(gpuDevice_->GetDevice());
	
	// SwapChainからResourceを引っ張ってくる
	backBuffers_.resize(2);
	for (int i = 0; i < backBuffers_.size(); i++) {
		// スワップチェーンからバッファを取得
		hr = swapChain_->GetBuffer(i, IID_PPV_ARGS(&backBuffers_[i]));
		assert(SUCCEEDED(hr));

		// RTVDescを受け取る
		D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = rtv_->GetDesc();
		// レンダーターゲットビューの生成
		device_->CreateRenderTargetView(backBuffers_[i].Get(), &rtvDesc, rtv_->GetCPUHandle(i));
	}
}