#include "RenderResource.h"

#include "../command/CommandManager.h"

using namespace LWP::Base;

RenderResource::RenderResource(ID3D12Device* device, CommandManager* manager) {
	// リソースの実体を作る
	CreateResource(device);
	// マネージャーのポインタを登録しておく
	manager_ = manager;
}

bool RenderResource::RegisterRTV() {
	// 登録済みならば戻る
	if (rtvIndex_ != -1) { return false; }

	// RTVに登録
	//rtvIndex_ = 
	return true;
}
bool RenderResource::ReRegisterRTV() {
	// 登録していないならば戻る
	if (rtvIndex_ == -1) { return false; }

	return true;
}
bool RenderResource::RegisterSRV() {
	// 登録済みならば戻る
	if (srvIndex_ != -1) { return false; }

	return true;
}
bool RenderResource::ReRegisterSRV() {
	// 登録していないならば戻る
	if (srvIndex_ == -1) { return false; }

	return true;
}
bool RenderResource::RegisterDSV() {
	// 登録済みならば戻る
	if (dsvIndex_ != -1) { return false; }

	return true;
}
bool RenderResource::ReRegisterDSV() {
	// 登録していないならば戻る
	if (dsvIndex_ == -1) { return false; }

	return true;
}

void RenderResource::SetResourceBarrier(D3D12_RESOURCE_BARRIER barrier) {
	
	// 現在のステータスを変更しておく
	currentBarrier_ = barrier;
}

void RenderResource::CreateResource(ID3D12Device* device) {
	// 1. Resourceの設定
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Width = UINT(width_);
	resourceDesc.Height = UINT(height_);
	resourceDesc.MipLevels = 0;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

	// 2．クリアカラーもセットしておく（今回は緑）
	D3D12_CLEAR_VALUE clearColor;
	clearColor.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	clearColor.Color[0] = 0.0f;
	clearColor.Color[1] = 1.0f;
	clearColor.Color[2] = 0.0f;
	clearColor.Color[3] = 1.0f;

	// 3. 利用するHeapの設定。非常に特殊な運用。
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_CUSTOM; // 細かい設定を行う
	heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK; // WriteBackポリシーでCPUアクセス可能
	heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_L0; // プロセッサの近くに配置

	// 4. Resourceを生成する
	ID3D12Resource* resource = nullptr;
	HRESULT hr = device->CreateCommittedResource(
		&heapProperties,					// Heapの設定
		D3D12_HEAP_FLAG_NONE,				// Heapの特殊な設定。特になし。
		&resourceDesc,						// Resourceの設定
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,	// 初回のResourceState。RTVで書き込むのでちょっとちがう設定
		&clearColor,							// Clear最適地。使わないならnullptr
		IID_PPV_ARGS(&resource)				// 作成するResourceポインタへのポインタ
	);
	assert(SUCCEEDED(hr));
}
