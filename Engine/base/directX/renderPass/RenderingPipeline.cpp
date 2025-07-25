#include "RenderingPipeline.h"

#include "base/WinApp.h"
#include "object/ObjectManager.h"
#include "resources/ResourceManager.h"
#include <Config.h>
namespace FPSPara = LWP::Config::FrameRate;

#include "component/Resource.h"
using namespace LWP;

namespace LWP::Base {
	void RenderingPipeline::Init() {
		HRESULT hr;

		// コマンド初期化
		commander_.Init(GPUDevice::GetInstance());
		SRV::GetInstance()->SetCommand(&commander_);	// コマンドクラスをセット

		defaultTexture_ = Resource::LoadTextureLongPath("resources/system/texture/white.png");	// デフォルトテクスチャを読み込み

		// スワップチェーンを生成する
		swapChainDesc_.Width = Config::Window::kResolutionWidth;		// 画面の幅。ウィンドウのクライアント領域を同じものにしておく
		swapChainDesc_.Height = Config::Window::kResolutionHeight;		// 画面の高さ。ウィンドウのクライアント領域を同じものにしておく 
		swapChainDesc_.Format = DXGI_FORMAT_R8G8B8A8_UNORM;				// 色の形式
		swapChainDesc_.SampleDesc.Count = 1;							// マルチサンプルしない
		swapChainDesc_.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;	// 描画のターゲットとして利用する
		swapChainDesc_.BufferCount = 2;									// ダブルバッファ
		swapChainDesc_.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;		// モニタにうつしたら、中身を廃棄
		// コマンドキュー、ウィンドウハンドル、設定を渡して生成する
		Microsoft::WRL::ComPtr<IDXGISwapChain1> swapChain1;
		hr = GPUDevice::GetInstance()->GetFactory()->CreateSwapChainForHwnd(commander_.Queue(), WinApp::GetInstance()->GetHWND(), &swapChainDesc_, nullptr, nullptr, &swapChain1);
		assert(SUCCEEDED(hr));
		// SwapChain4を得る
		swapChain1->QueryInterface(IID_PPV_ARGS(&swapChain_));

		// バックバッファ生成
		for (int i = 0; i < 2; i++) {
			backBuffers_.push_back(BackBuffer(swapChain_.Get(), i));
			// 初期化
			backBuffers_.back().Init();
		}

		// パイプラインの要素を初期化
		drawSolid_.Init();
		drawPlane_.Init();
		deferredShading_.Init();
		imguiRender_.Init();
		dirShadow_.Init();
	}
	void RenderingPipeline::DrawCall() {
		// 命令積み込み開始
		ID3D12GraphicsCommandList6* list = commander_.List();
		// バックバッファを取得
		BackBuffer* back = &backBuffers_[swapChain_->GetCurrentBackBufferIndex()];
		
		// 初期設定
		
		// 形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけば良い
		list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		// 描画用のSRVのDescriptorHeapを設定
		ID3D12DescriptorHeap* descriptorHeaps[] = { SRV::GetInstance()->GetHeap() };
		list->SetDescriptorHeaps(1, descriptorHeaps);

		renderClear_.ClearAllCamera(list);	// カメラオールクリア

		// コマンドを積み込む
		drawSolid_.PushCommand(list);
		dirShadow_.PushCommand(list);
		deferredShading_.PushCommand(list);
		drawPlane_.PushCommand(list);

		// メインカメラの描画をバックバッファーにコピー
		renderClear_.PushCommand(back, list);
		copy_.PushCommand(Object::Manager::GetInstance()->GetMainCamera()->GetTextureResource(), back, list);
		back->ChangeResourceBarrier(D3D12_RESOURCE_STATE_PRESENT, list);	// バリアを元に戻す
		// ImGuiの描画命令を積み込む
		imguiRender_.PushCommand(back, list);

		// コマンドを実行
		commander_.Execute();
		// GPUとOSに画面の交換を行うよう通知する
		swapChain_->Present(FPSPara::kVsync, 0);	// 垂直同期をする際は左の数字を1にする
	}
}