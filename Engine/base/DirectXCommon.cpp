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

	// HeapManager作成
	heaps_ = std::make_unique<HeapManager>(winApp->GetHWND(), gpuDevice_.get(), backBufferWidth, backBufferHeight, commandManager_->GetQueue());


	// ディスクリプタヒープを登録
	commandManager_->SetDescriptorHeap(heaps_.get());
}

void DirectXCommon::PreDraw() {
	// これから書き込むバックバッファのインデックスを取得
	commandManager_->PreDraw();
}

void DirectXCommon::DrawCall() {
	commandManager_->DrawCall();
}

void DirectXCommon::PostDraw() {
	// これから書き込むバックバッファのインデックスを取得
	commandManager_->PostDraw();
	// 描画カウントリセット
	commandManager_->Reset();
}