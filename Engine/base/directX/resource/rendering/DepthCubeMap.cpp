#include "DepthCubeMap.h"
#include "resources/texture/Texture.h"

using namespace LWP;
using namespace LWP::Base;

void DepthCubeMap::Init(GPUDevice* device, HeapManager* heaps) {
	HRESULT hr = S_FALSE;
	width = 1024 / 2;
	height = 1024 / 2;

	// 1. Resourceの設定
	desc.Width = UINT(width);
	desc.Height = UINT(height);
	desc.MipLevels = 0;
	desc.DepthOrArraySize = 6;
	desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	desc.SampleDesc.Count = 1;
	desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	// 2．クリアカラー
	clearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	clearValue.DepthStencil.Depth = 1.0f; // 1.0f(最大値)でクリア

	// 3. 利用するHeapの設定。非常に特殊な運用。
	properties.Type = D3D12_HEAP_TYPE_DEFAULT; // デフォルト

	// 4. バリアを設定
	currentBarrierState = D3D12_RESOURCE_STATE_DEPTH_WRITE;

	// 5. Resourceを生成する
	hr = device->GetDevice()->CreateCommittedResource(
		&properties,			// Heapの設定
		D3D12_HEAP_FLAG_NONE,	// Heapの特殊な設定。特になし。
		&desc,					// Resourceの設定
		currentBarrierState,	// 初回のResourceState
		&clearValue,			// Clear最適地。使わないならnullptr
		IID_PPV_ARGS(&resource_)	// 作成するResourceポインタへのポインタ
	);
	assert(SUCCEEDED(hr));

	// RTV上に登録
	dsvInfos = heaps->dsv()->CreateDepthStencilCubeMap(resource_.Get());
}

void DepthCubeMap::Clear(ID3D12GraphicsCommandList* list) {
	for (int i = 0; i < 6; i++) {
		// 指定した深度で画面全体をクリアする
		list->ClearDepthStencilView(dsvInfos[i].cpuView, D3D12_CLEAR_FLAG_DEPTH, clearValue.DepthStencil.Depth, 0, 0, nullptr);
	}
};

//RenderResource::operator Resource::Texture() { return *this; }
