#include "RenderResource.h"
#include "resources/texture/Texture.h"

using namespace LWP;
using namespace LWP::Base;

void RenderResource::Init(GPUDevice* device, HeapManager* heaps) {
	HRESULT hr = S_FALSE;
	
	// 1. Resourceの設定
	desc.Width = UINT(width);
	desc.Height = UINT(height);
	desc.MipLevels = 0;
	desc.DepthOrArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	desc.SampleDesc.Count = 1;
	desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

	// 2．クリアカラー
	clearValue.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	clearValue.Color[0] = 0.1f;
	clearValue.Color[1] = 0.25f;
	clearValue.Color[2] = 0.5f;
	clearValue.Color[3] = 1.0f;

	// 3. 利用するHeapの設定。非常に特殊な運用。
	properties.Type = D3D12_HEAP_TYPE_DEFAULT; // デフォルト
	//properties.Type = D3D12_HEAP_TYPE_CUSTOM; // 細かい設定を行う
	//properties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK; // WriteBackポリシーでCPUアクセス可能
	//properties.MemoryPoolPreference = D3D12_MEMORY_POOL_L0; // プロセッサの近くに配置

	// 4. バリアを設定
	currentBarrierState = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;

	// 5. Resourceを生成する
	hr = device->GetDevice()->CreateCommittedResource(
		&properties,			// Heapの設定
		D3D12_HEAP_FLAG_NONE,	// Heapの特殊な設定。特になし。
		&desc,					// Resourceの設定
		currentBarrierState,	// 初回のResourceState。RTVで書き込むのでちょっとちがう設定
		&clearValue,			// Clear最適地。使わないならnullptr
		IID_PPV_ARGS(&resource_)	// 作成するResourceポインタへのポインタ
	);
	assert(SUCCEEDED(hr));

	// RTV上に登録
	rtvInfo = heaps->rtv()->CreateRenderTargetView(resource_.Get());
	// SRV上に登録
	srvInfo = heaps->srv()->CreateRenderResource(resource_.Get());
}

void RenderResource::Clear(ID3D12GraphicsCommandList* list) {
	list->ClearRenderTargetView(rtvInfo.cpuView, clearValue.Color, 0, nullptr);
};

//RenderResource::operator Resource::Texture() { return *this; }
