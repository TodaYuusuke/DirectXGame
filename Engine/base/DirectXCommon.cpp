#include "DirectXCommon.h"
#include "object/core/Camera.h"
#include "../utility/MyUtility.h"

#include <Config.h>
namespace FPSPara = LWP::Config::FrameRate;

#include <cassert>
#include <format>

using namespace Microsoft::WRL;
using namespace LWP::Base;
using namespace LWP::Utility;

DirectXCommon::DirectXCommon() {}

void DirectXCommon::Init() {
	HRESULT hr = S_FALSE;

	// デバイスを変数に受け取る
	GPUDevice* gpu = GPUDevice::GetInstance();

	// レンダラー初期化
	renderer_ = std::make_unique<RendererManager>();
	renderer_->Init();

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
	hr = gpu->GetFactory()->CreateSwapChainForHwnd(renderer_->GetCommand()->Queue(), WinApp::GetInstance()->GetHWND(), &swapChainDesc_, nullptr, nullptr, &swapChain1);
	assert(SUCCEEDED(hr));
	// SwapChain4を得る
	swapChain1->QueryInterface(IID_PPV_ARGS(&swapChain_));

	// バックバッファ生成
	for (int i = 0; i < 2; i++) {
		backBuffers_.push_back(BackBuffer(swapChain_.Get(), i));
		// 初期化
		backBuffers_.back().Init();
	}
	// 深度マップ生成
	depthStencil_.Init();
}

void DirectXCommon::SetMainCamera(Object::Camera* camera) {
	// ポストプロセスを行うか確認
	if (camera->pp.use) {
		// カメラのリソースに書き込み
		renderer_->AddTarget(camera->GetBufferView(), camera->GetRenderResource(), &depthStencil_);
		// ポストプロセス用のターゲットセット
		renderer_->AddTarget(camera->GetRenderResource(), &backBuffers_[swapChain_->GetCurrentBackBufferIndex()], &depthStencil_, &camera->pp);
	}
	// しないならば
	else {
		// 直接BackBufferに書き込み
		renderer_->AddTarget(camera->GetBufferView(), &backBuffers_[swapChain_->GetCurrentBackBufferIndex()], &depthStencil_);
	}

	// SpriteをBackBufferに
	renderer_->AddSpriteTarget(camera->GetBufferView(), &backBuffers_[swapChain_->GetCurrentBackBufferIndex()], &depthStencil_);
}

void DirectXCommon::DrawCall() {
	renderer_->DrawCall();
	// GPUとOSに画面の交換を行うよう通知する
	swapChain_->Present(FPSPara::kVsync, 0);	// 垂直同期をする際は左の数字を1にする
}

void DirectXCommon::DebugGUI() {
	if (ImGui::BeginTabItem("Base")) {
		RTV::GetInstance()->DebugGUI();
		SRV::GetInstance()->DebugGUI();
		DSV::GetInstance()->DebugGUI();
		ImGui::EndTabItem();
	}
}