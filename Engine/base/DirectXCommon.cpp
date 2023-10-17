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

	// コマンド関連初期化
	InitializeCommand();

	// スワップチェーンの生成
	CreateSwapChain();

	// レンダーターゲット生成
	CreateFinalRenderTargets();

	// フェンス生成
	CreateFence();

	// DSV（深度バッファ）作成
	dsv_ = std::make_unique<DSV>();
	dsv_->Initialize(gpuDevice_->GetDevice(), backBufferWidth_, backBufferHeight_);
}

void DirectXCommon::PreDraw() {
	// これから書き込むバックバッファのインデックスを取得
	UINT backBufferIndex = swapChain_->GetCurrentBackBufferIndex();

	// TransitionBarrierの設定
	D3D12_RESOURCE_BARRIER barrier = MakeResourceBarrier(
		backBuffers_[backBufferIndex].Get(),
		D3D12_RESOURCE_STATE_PRESENT,
		D3D12_RESOURCE_STATE_RENDER_TARGET
	);
	commandList_->ResourceBarrier(1, &barrier);

	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = rtv_->GetCPUHandle(backBufferIndex);
	// レンダーターゲットビュー用ディスクリプタヒープのハンドルを取得
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = dsv_->GetCPUHandle(0);
	// 描画先のRTVとDSVを設定する
	commandList_->OMSetRenderTargets(1, &rtvHandle, false, &dsvHandle);
	
	// 全画面クリア
	rtv_->ClearRenderTarget(backBufferIndex, commandList_.Get());
	// 指定した深度で画面全体をクリアする
	dsv_->ClearDepth(commandList_.Get());

	// 描画用のSRVのDescriptorHeapを設定
	ID3D12DescriptorHeap* descriptorHeaps[] = { srv_->GetHeap() };
	commandList_->SetDescriptorHeaps(1, descriptorHeaps);

	// ビューポート
	D3D12_VIEWPORT viewport = {};
	// クライアント領域のサイズと一緒にして画面全体に表示
	viewport.Width = static_cast<float>(winApp_->GetClientWidth());
	viewport.Height = static_cast<float>(winApp_->GetClientHeight());
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	// viewportを設定
	commandList_->RSSetViewports(1, &viewport);

	// シザー矩形
	D3D12_RECT scissorRect = {};
	// 基本的にビューポートと同じ矩形が構成されるようにする
	scissorRect.left = 0;
	scissorRect.right = winApp_->GetClientWidth();
	scissorRect.top = 0;
	scissorRect.bottom = winApp_->GetClientHeight();
	// Scirssorを設定
	commandList_->RSSetScissorRects(1, &scissorRect);
}

void DirectXCommon::PostDraw() {
	HRESULT hr = S_FALSE;

	// これから書き込むバックバッファのインデックスを取得
	UINT backBufferIndex = swapChain_->GetCurrentBackBufferIndex();

	// TransitionBarrierの設定
	D3D12_RESOURCE_BARRIER barrier = MakeResourceBarrier(
		backBuffers_[backBufferIndex].Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_PRESENT
	);
	commandList_->ResourceBarrier(1, &barrier);


	// コマンドリストの内容を確定させる。全てのコマンドを積んでからcloseすること
	hr = commandList_->Close();
	assert(SUCCEEDED(hr));


	// GPUにコマンドリストの実行を行わせる
	ID3D12CommandList* commandLists[] = { commandList_.Get() };
	commandQueue_->ExecuteCommandLists(1, commandLists);


	// GPUとOSに画面の交換を行うよう通知する
	swapChain_->Present(1, 0);

	// GPUがここまでたどり着いたときに、Fenceの値を指定した値に代入するようにSignalを送る
	commandQueue_->Signal(fence_.Get(), ++fenceVal_);
	if (fence_->GetCompletedValue() != fenceVal_) {
		HANDLE event = CreateEvent(NULL, FALSE, FALSE, NULL);
		assert(event != nullptr);
		fence_->SetEventOnCompletion(fenceVal_, event);
		WaitForSingleObject(event, INFINITE);
		CloseHandle(event);
	}

	// 次のフレーム用のコマンドリストを準備
	hr = commandAllocateor_->Reset();
	assert(SUCCEEDED(hr));
	hr = commandList_->Reset(commandAllocateor_.Get(), nullptr);
	assert(SUCCEEDED(hr));
}

void DirectXCommon::InitializeCommand() {
	HRESULT hr = S_FALSE;

	// コマンドキューを生成する
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};
	hr = device_->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&commandQueue_));
	// コマンドキューの生成がうまくいかなかったので起動できない
	assert(SUCCEEDED(hr));

	// コマンドアロケーターを生成する
	hr = device_->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocateor_));
	// コマンドアロケーターの生成がうまくいかなかったので起動できない
	assert(SUCCEEDED(hr));

	// コマンドリストを生成する
	hr = device_->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocateor_.Get(), nullptr, IID_PPV_ARGS(&commandList_));
	// コマンドリストの生成がうまくいかなかったので起動できない
	assert(SUCCEEDED(hr));
}

void DirectXCommon::CreateSwapChain() {
	HRESULT hr = S_FALSE;

	// スワップチェーンを生成する
	swapChainDesc_.Width = winApp_->GetClientWidth();				// 画面の幅。ウィンドウのクライアント領域を同じものにしておく
	swapChainDesc_.Height = winApp_->GetClientHeight();				// 画面の高さ。ウィンドウのクライアント領域を同じものにしておく 
	swapChainDesc_.Format = DXGI_FORMAT_R8G8B8A8_UNORM;				// 色の形式
	swapChainDesc_.SampleDesc.Count = 1;								// マルチサンプルしない
	swapChainDesc_.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;	// 描画のターゲットとして利用する
	swapChainDesc_.BufferCount = 2;									// ダブルバッファ
	swapChainDesc_.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;		// モニタにうつしたら、中身を廃棄
	// コマンドキュー、ウィンドウハンドル、設定を渡して生成する
	ComPtr<IDXGISwapChain1> swapChain1;
	hr = gpuDevice_->GetFactory()->CreateSwapChainForHwnd(commandQueue_.Get(), winApp_->GetHWND(), &swapChainDesc_, nullptr, nullptr, &swapChain1);
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

void DirectXCommon::CreateFence() {
	HRESULT hr = S_FALSE;

	// 初期値0でFenceを作る
	fenceVal_ = 0;
	hr = device_->CreateFence(fenceVal_, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence_));
	assert(SUCCEEDED(hr));
}


D3D12_RESOURCE_BARRIER DirectXCommon::MakeResourceBarrier(ID3D12Resource* pResource, D3D12_RESOURCE_STATES stateBefore, D3D12_RESOURCE_STATES stateAfter) {

	// TransitionBarrierの設定
	D3D12_RESOURCE_BARRIER barrier{};
	// 今回のバリアはTransition
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	// Noneにしておく
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	// バリアを張る対象のリソース。現在のバックバッファに対して行う
	barrier.Transition.pResource = pResource;
	// 遷移前（現在）のResourceState
	barrier.Transition.StateBefore = stateBefore;
	// 遷移前（現在）のResourceState
	barrier.Transition.StateAfter = stateAfter;

	return barrier;
}

ID3D12DescriptorHeap* DirectXCommon::CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible) {
	HRESULT hr = S_FALSE;

	ID3D12DescriptorHeap* descriptorHeap = nullptr;
	D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc{};
	descriptorHeapDesc.Type = heapType;
	descriptorHeapDesc.NumDescriptors = numDescriptors;
	descriptorHeapDesc.Flags = shaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	hr = gpuDevice_->GetDevice()->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&descriptorHeap));
	assert(SUCCEEDED(hr));
	return descriptorHeap;
}