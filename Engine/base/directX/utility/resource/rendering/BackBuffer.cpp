#include "BackBuffer.h"

using namespace LWP::Base;

BackBuffer::BackBuffer(IDXGISwapChain4* swapChain, int index) {
	HRESULT hr = S_FALSE;

	// スワップチェーンからバッファを取得
	hr = swapChain->GetBuffer(index, IID_PPV_ARGS(&resource_));
	assert(SUCCEEDED(hr));
}

void BackBuffer::Init(GPUDevice* device, HeapManager* heaps) {
	HRESULT hr = S_FALSE;

	// クリアカラー設定
	clearValue.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	clearValue.Color[0] = 0.1f;
	clearValue.Color[1] = 0.25f;
	clearValue.Color[2] = 0.5f;
	clearValue.Color[3] = 1.0f;
	// バリアの初期値セット
	currentBarrierState = D3D12_RESOURCE_STATE_PRESENT;

	// リソース生成はスワップチェーンから引っ張ってきたので不要
	device;

	// RTV上に登録
	rtvInfo = heaps->rtv()->CreateRenderTargetView(resource_.Get());
}


void BackBuffer::Clear(ID3D12GraphicsCommandList* list) {
	list->ClearRenderTargetView(rtvInfo.cpuView, clearValue.Color, 0, nullptr);
};
