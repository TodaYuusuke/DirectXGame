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

	// RTV作成
	rtv_ = std::make_unique<RTV>();
	rtv_->Initialize(winApp->GetHWND(), gpuDevice_.get(), backBufferWidth_, backBufferHeight, commandManager_->GetQueue());

	// SRV作成
	srv_ = std::make_unique<SRV>();
	srv_->Initialize(gpuDevice_->GetDevice());

	// DSV（深度バッファ）作成
	dsv_ = std::make_unique<DSV>();
	dsv_->Initialize(gpuDevice_->GetDevice(), backBufferWidth_, backBufferHeight_, srv_.get());

	// ディスクリプタヒープを登録
	commandManager_->SetDescriptorHeap(rtv_.get(), dsv_.get(), srv_.get());
}

void DirectXCommon::PreDraw() {
	// これから書き込むバックバッファのインデックスを取得
	commandManager_->PreDraw();
}

void DirectXCommon::PostDraw() {
	// これから書き込むバックバッファのインデックスを取得
	commandManager_->PostDraw();
	// 描画カウントリセット
	commandManager_->Reset();
}