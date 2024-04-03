#include "DirectXCommon.h"
#include "../utility/MyUtility.h"

#include <Config.h>
namespace FPSPara = LWP::Config::FrameRate;

#include <cassert>
#include <format>

using namespace Microsoft::WRL;
using namespace LWP::Base;
using namespace LWP::Utility;

void DirectXCommon::Initialize(WinApp* winApp, int32_t backBufferWidth, int32_t backBufferHeight) {
	HRESULT hr = S_FALSE;

	winApp_ = winApp;
	backBufferWidth_ = backBufferWidth;
	backBufferHeight_ = backBufferHeight;

	// デバイス初期化
	gpuDevice_ = std::make_unique<GPUDevice>();
	gpuDevice_->Init();
	device_ = gpuDevice_->GetDevice();

	// コマンド管理初期化
	commandManager_ = std::make_unique<CommandManager>();
	commandManager_->Initialize(gpuDevice_->GetDevice());

	// スワップチェーンを生成する
	swapChainDesc_.Width = Config::Window::kResolutionWidth;		// 画面の幅。ウィンドウのクライアント領域を同じものにしておく
	swapChainDesc_.Height = Config::Window::kResolutionHeight;		// 画面の高さ。ウィンドウのクライアント領域を同じものにしておく 
	swapChainDesc_.Format = DXGI_FORMAT_R8G8B8A8_UNORM;				// 色の形式
	swapChainDesc_.SampleDesc.Count = 1;							// マルチサンプルしない
	swapChainDesc_.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;	// 描画のターゲットとして利用する
	swapChainDesc_.BufferCount = 2;									// ダブルバッファ
	swapChainDesc_.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;		// モニタにうつしたら、中身を廃棄
	// コマンドキュー、ウィンドウハンドル、設定を渡して生成する
	ComPtr<IDXGISwapChain1> swapChain1;
	hr = gpuDevice_->GetFactory()->CreateSwapChainForHwnd(commandManager_.get()->GetQueue(), winApp->GetHWND(), &swapChainDesc_, nullptr, nullptr, &swapChain1);
	assert(SUCCEEDED(hr));
	// SwapChain4を得る
	swapChain1->QueryInterface(IID_PPV_ARGS(&swapChain_));

	// HeapManager作成
	heaps_ = std::make_unique<HeapManager>(winApp->GetHWND(), gpuDevice_.get(), backBufferWidth, backBufferHeight, commandManager_->GetQueue());

	// バックバッファ生成
	for (int i = 0; i < 2; i++) {
		backBuffers_.push_back(BackBuffer(swapChain_.Get(), i));
		// 初期化
		backBuffers_.back().Init(gpuDevice_.get(), heaps_.get());
	}
	// 深度マップ生成
	depthStencil_.Init(gpuDevice_.get(), heaps_.get());

	// ディスクリプタヒープを登録
	commandManager_->SetDescriptorHeap(heaps_.get());
}

void DirectXCommon::PreDraw() {
	// これから書き込むバックバッファのインデックスを取得
	commandManager_->PreDraw();
}

void DirectXCommon::DrawCall() {
	commandManager_->DrawCall();

	// GPUとOSに画面の交換を行うよう通知する
	swapChain_->Present(FPSPara::kVsync, 0);	// 垂直同期をする際は左の数字を1にする
}

void DirectXCommon::PostDraw() {
	// これから書き込むバックバッファのインデックスを取得
	commandManager_->PostDraw();
	// 描画カウントリセット
	commandManager_->Reset();
}