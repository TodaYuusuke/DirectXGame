#include "RTV.h"

#include <Config.h>

using namespace LWP::Base;
using namespace Microsoft::WRL;

void RTV::Initialize(HWND hwnd, GPUDevice* gpuDevice, int32_t width, int32_t height, ID3D12CommandQueue* queue) {
	HRESULT hr = S_FALSE;

	device_ = gpuDevice->GetDevice();
	// 複数画面の書き込み先 + メイン描画用(ダブルバッファ)
	size_ = lwpC::Rendering::kMaxMultiWindowRendering + 2;
	// 1つ分のサイズを計算
	kDescriptorSize_ = device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	// RTV用のヒープでディスクリプタの数は2。RTVはShader内で触るものではないので、ShaderVisibleはfalse
	heap_ = CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, size_, false);

	// レンダーターゲットビューの設定
	// シェーダーの計算結果をSRGBに変換して書き込む
	desc_.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	desc_.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;


	// スワップチェーンを生成する
	swapChainDesc_.Width = width;				// 画面の幅。ウィンドウのクライアント領域を同じものにしておく
	swapChainDesc_.Height = height;				// 画面の高さ。ウィンドウのクライアント領域を同じものにしておく 
	swapChainDesc_.Format = DXGI_FORMAT_R8G8B8A8_UNORM;				// 色の形式
	swapChainDesc_.SampleDesc.Count = 1;							// マルチサンプルしない
	swapChainDesc_.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;	// 描画のターゲットとして利用する
	swapChainDesc_.BufferCount = 2;									// ダブルバッファ
	swapChainDesc_.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;		// モニタにうつしたら、中身を廃棄
	// コマンドキュー、ウィンドウハンドル、設定を渡して生成する
	ComPtr<IDXGISwapChain1> swapChain1;
	hr = gpuDevice->GetFactory()->CreateSwapChainForHwnd(queue, hwnd, &swapChainDesc_, nullptr, nullptr, &swapChain1);
	assert(SUCCEEDED(hr));

	// SwapChain4を得る
	swapChain1->QueryInterface(IID_PPV_ARGS(&swapChain_));


	// SwapChainからResourceを引っ張ってくる
	backBuffers_.resize(2);
	for (int i = 0; i < backBuffers_.size(); i++) {
		// スワップチェーンからバッファを取得
		hr = swapChain_->GetBuffer(i, IID_PPV_ARGS(&backBuffers_[i]));
		assert(SUCCEEDED(hr));

		// レンダーターゲットビューの生成
		device_->CreateRenderTargetView(backBuffers_[i].Get(), &desc_, GetCPUHandle(i));
	}
}

void RTV::ClearRenderTarget(ID3D12GraphicsCommandList* commandList) {
	// レンダーターゲットビュー用ディスクリプタヒープのハンドルを取得し、指定した色で画面全体をクリアする
	float clearColor[] = { 0.1f,0.25f,0.5f,1.0f };	// 青っぽい色。RGBAの順
	commandList->ClearRenderTargetView(GetCPUHandle(swapChain_->GetCurrentBackBufferIndex()), clearColor, 0, nullptr);
}

int RTV::CreateRenderTargetView(ID3D12Resource* resource) {
	device_->CreateRenderTargetView(resource, &desc_, GetCPUHandle(GetCount()));
	return GetAndIncrement();
}
