#include "ShadowMapping.h"
#include "../../descriptorHeap/RTV.h"
#include "../../descriptorHeap/DSV.h"
#include "../../descriptorHeap/SRV.h"

#include <Config.h>

#include <Adapter.h>

using namespace LWP;
using namespace LWP::Base;

void ShadowMapping::SetDrawTarget(const Math::Matrix4x4& vp, ID3D12Resource* resource, uint32_t dsvIndex) {
	*vpResourceBuffer_->data_ = vp;
	resource_ = resource;
	dsvIndex_ = dsvIndex;
}

void ShadowMapping::PreDraw(ID3D12GraphicsCommandList* list) {
	// シャドウマップ用のDSVハンドルを取得
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = dsv_->GetCPUHandle(dsvIndex_);
	// 描画先のDSVを設定する
	list->OMSetRenderTargets(0, nullptr, false, &dsvHandle);

	// TransitionBarrierの設定
	D3D12_RESOURCE_BARRIER barrier = MakeResourceBarrier(
		resource_,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		D3D12_RESOURCE_STATE_DEPTH_WRITE
	);

	// リソースバリアをセット
	list->ResourceBarrier(1, &barrier);

	// 指定した深度で画面全体をクリアする
	dsv_->ClearDepth(dsvIndex_, list);

	// 描画用のSRVのDescriptorHeapを設定
	ID3D12DescriptorHeap* descriptorHeaps[] = { srv_->GetHeap() };
	list->SetDescriptorHeaps(1, descriptorHeaps);

	// ビューポート
	D3D12_VIEWPORT viewport = {};
	// シャドウマップ用のテクスチャと同じサイズにする
	viewport.Width = 1024.0f * lwpC::Shadow::kResolutionScale;
	viewport.Height = 1024.0f * lwpC::Shadow::kResolutionScale;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	// viewportを設定
	list->RSSetViewports(1, &viewport);

	// シザー矩形
	D3D12_RECT scissorRect = {};
	// 基本的にビューポートと同じ矩形が構成されるようにする
	scissorRect.left = 0;
	scissorRect.right = 1024 * static_cast<int>(lwpC::Shadow::kResolutionScale);
	scissorRect.top = 0;
	scissorRect.bottom = 1024 * static_cast<int>(lwpC::Shadow::kResolutionScale);
	// Scirssorを設定
	list->RSSetScissorRects(1, &scissorRect);
}

void ShadowMapping::PostDraw(ID3D12GraphicsCommandList* list) {
	// TransitionBarrierの設定
	D3D12_RESOURCE_BARRIER barrier = MakeResourceBarrier(
		resource_,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		D3D12_RESOURCE_STATE_GENERIC_READ
	);

	// リソースバリアをセット
	list->ResourceBarrier(1, &barrier);
}

void ShadowMapping::CreatePSO(ID3D12Device* device, DXC* dxc, ID3D12RootSignature* rootSignature) {
	pso_ = std::make_unique<PSO>();
	pso_->InitializeForShadow(device, rootSignature, dxc);
}