#include "DepthStencil.h"

using namespace LWP::Base;

void DepthStencil::Init(GPUDevice* device, HeapManager* heaps) {
	HRESULT hr = S_FALSE;

	// 1. Resourceの設定
	desc.Width = UINT(width);	// Textureの幅
	desc.Height = UINT(height);	// Textureの高さ
	desc.MipLevels = 1; // mipmapの数
	desc.DepthOrArraySize = 1; // 奥行き or 配列Textureの配列数
	desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; // DepthStencilとして利用可能なフォーマット
	desc.SampleDesc.Count = 1; // サンプリングカウント、1固定
	desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D; // 2次元
	desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL; // DepthStencilとして使う通知

	// 2．クリア値
	clearValue.DepthStencil.Depth = 1.0f; // 1.0f(最大値)でクリア
	clearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

	// 3. 利用するHeapの設定。
	properties.Type = D3D12_HEAP_TYPE_DEFAULT; // VRAM上に作る

	// 4. バリアを設定（深度値を書き込む状態にしておく）
	currentBarrierState = D3D12_RESOURCE_STATE_DEPTH_WRITE;

	// 5. Resourceを生成する
	hr = device->GetDevice()->CreateCommittedResource(
		&properties,			// Heapの設定
		D3D12_HEAP_FLAG_NONE,	// Heapの特殊な設定。特になし。
		&desc,					// Resourceの設定
		currentBarrierState,	// バリアの設定
		&clearValue,				// Clear最適地。使わないならnullptr
		IID_PPV_ARGS(&resource_)	// 作成するResourceポインタへのポインタ
	);
	assert(SUCCEEDED(hr));

	// DSV上に登録
	dsvInfo = heaps->dsv()->CreateDepthStencilView(resource_.Get());
}

void DepthStencil::Clear(ID3D12GraphicsCommandList* list) {
	// 指定した深度で画面全体をクリアする
	list->ClearDepthStencilView(dsvInfo.cpuView, D3D12_CLEAR_FLAG_DEPTH, clearValue.DepthStencil.Depth, 0, 0, nullptr);
}

