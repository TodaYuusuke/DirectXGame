#include "RenderResource.h"

#include "../command/CommandManager.h"

using namespace LWP::Base;

RenderResource::RenderResource(ID3D12Device* device, HeapManager* heaps, const int width, const int height) {
	width_ = width;
	height_ = height;
	// リソースの実体を作る
	CreateResource(device);
	// heapマネージャーのポインタを登録しておく
	heaps_ = heaps;
}

void RenderResource::SetResolution(const int width, const int height) {
	width_ = width;
	height_ = height;

	// リソース再生成
	//ReRegisterRTV();
	//ReRegisterSRV();
	//ReRegisterDSV();
}

bool RenderResource::RegisterRTV() {
	// 登録済みならば戻る
	if (rtvIndex_ != -1) { return false; }

	// RTVに登録
	rtvIndex_ = heaps_->rtv()->CreateRenderTargetView(resource_.Get());
	return true;
}
bool RenderResource::RegisterSRV() {
	// 登録済みならば戻る
	if (srvIndex_ != -1) { return false; }

	// SRVに登録
	srvIndex_ = heaps_->srv()->CreateShaderResourceView(resource_.Get(), width_, height_);
	return true;
}
bool RenderResource::RegisterDSV() {
	// 登録済みならば戻る
	if (dsvIndex_ != -1) { return false; }

	// DSVに登録
	depthMapResource_ = heaps_->dsv()->CreateDepthStencilResource(width_, height_);
	dsvIndex_ = heaps_->dsv()->CreateDepthStencil(depthMapResource_.Get());
	return true;
}

bool RenderResource::RegisterDepthToSRV() {
	// 登録済みならば戻る
	if (depthSRVIndex_ != -1) { return false; }

	// SRVに登録
	depthSRVIndex_ = heaps_->srv()->UploadDepthMap(depthMapResource_.Get());
	return true;
}

void RenderResource::SetResourceBarrier(D3D12_RESOURCE_STATES state, ID3D12GraphicsCommandList* list) {
	// リソースバリアを生成
	D3D12_RESOURCE_BARRIER barrier = CreateResourceBarrier(resource_.Get(), currentBarrierState_, state);
	list->ResourceBarrier(1, &barrier);
	// 現在のステータスを変更しておく
	currentBarrierState_ = state;
}
void RenderResource::SetDepthMapResourceBarrier(D3D12_RESOURCE_STATES state, ID3D12GraphicsCommandList* list) {
	// リソースバリアを生成
	D3D12_RESOURCE_BARRIER barrier = CreateResourceBarrier(depthMapResource_.Get(), currentDepthMapBarrierState_, state);
	list->ResourceBarrier(1, &barrier);
	// 現在のステータスを変更しておく
	currentDepthMapBarrierState_ = state;
}

bool RenderResource::ReRegisterRTV() {
	// 登録していないならば戻る
	if (rtvIndex_ == -1) { return false; }

	// 処理は未実装
	return true;
}
bool RenderResource::ReRegisterSRV() {
	// 登録していないならば戻る
	if (srvIndex_ == -1) { return false; }

	// 処理は未実装
	return true;
}
bool RenderResource::ReRegisterDSV() {
	// 登録していないならば戻る
	if (dsvIndex_ == -1) { return false; }

	// 処理は未実装
	return true;
}

void RenderResource::CreateResource(ID3D12Device* device) {
	HRESULT hr = S_FALSE;
	
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
	hr = device->CreateCommittedResource(
		&heapProperties,					// Heapの設定
		D3D12_HEAP_FLAG_NONE,				// Heapの特殊な設定。特になし。
		&resourceDesc,						// Resourceの設定
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,	// 初回のResourceState。RTVで書き込むのでちょっとちがう設定
		&clearColor,							// Clear最適地。使わないならnullptr
		IID_PPV_ARGS(&resource_)				// 作成するResourceポインタへのポインタ
	);
	assert(SUCCEEDED(hr));
}

D3D12_RESOURCE_BARRIER RenderResource::CreateResourceBarrier(ID3D12Resource* resoruce, D3D12_RESOURCE_STATES preState, D3D12_RESOURCE_STATES state) {
	// TransitionBarrierの設定
	D3D12_RESOURCE_BARRIER barrier{};
	// 今回のバリアはTransition
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	// Noneにしておく
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	// 全てのサブリソースを選択
	barrier.Transition.Subresource = 0xFFFFFFFF;
	// バリアを張る対象のリソース
	barrier.Transition.pResource = resoruce;
	// 遷移前（現在）のResourceState
	barrier.Transition.StateBefore = preState;
	// 遷移後のResourceState
	barrier.Transition.StateAfter = state;

	return barrier;
}