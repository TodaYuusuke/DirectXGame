#include "ShadowMapCommand.h"
#include "../../descriptorHeap/RTV.h"
#include "../../descriptorHeap/DSV.h"
#include "../../descriptorHeap/SRV.h"

#include <Config.h>
namespace RenderingPara = LWP::Config::Rendering;

#include <Adapter.h>

using namespace LWP::Base;

void ShadowMapCommand::InitializePreDraw() {
	// 初期化
	structCount_->directionLight = 0;
	structCount_->pointLight = 0;
	// レンダリング回数（本当はpointCount * 6）
	multiRenderingCount_ = *dirCount + *pointCount * 6;
}

void ShadowMapCommand::PreDraw(ID3D12GraphicsCommandList* list) {
	// このレンダリングで描画する対象をセットする
	if (currentRenderingCount_ < *dirCount) {
		resource_ = direction_->shadowMap_[currentRenderingCount_].resource_.Get();
		index_ = direction_->shadowMap_[currentRenderingCount_].dsvIndex_;
		structCount_->directionLight++;
	}
	else if (currentRenderingCount_ - *dirCount < *pointCount * 6) {
		int index = currentRenderingCount_ - *dirCount;
		resource_ = point_->shadowMap_[index / 6].resource_.Get();
		index_ = point_->shadowMap_[index / 6].dsvIndex_[index % 6];
		structCount_->pointLight++;
	}


	// TransitionBarrierの設定
	D3D12_RESOURCE_BARRIER barrier = MakeResourceBarrier(
		resource_,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		D3D12_RESOURCE_STATE_DEPTH_WRITE
	);

	// リソースバリアをセット
	list->ResourceBarrier(1, &barrier);

	// シャドウマップ用のDSVハンドルを取得
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = dsv_->GetCPUHandle(index_);
	// 描画先のDSVを設定する
	list->OMSetRenderTargets(0, nullptr, false, &dsvHandle);

	// 指定した深度で画面全体をクリアする（1はシャドウマップ用DSV）
	dsv_->ClearDepth(index_, list);

	// 描画用のSRVのDescriptorHeapを設定
	ID3D12DescriptorHeap* descriptorHeaps[] = { srv_->GetHeap() };
	list->SetDescriptorHeaps(1, descriptorHeaps);

	// ビューポート
	D3D12_VIEWPORT viewport = {};
	// シャドウマップ用のテクスチャと同じサイズにする
	viewport.Width = 1024.0f * RenderingPara::kShadowMapResolutionScale;
	viewport.Height = 1024.0f * RenderingPara::kShadowMapResolutionScale;
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
	scissorRect.right = 1024 * static_cast<int>(RenderingPara::kShadowMapResolutionScale);
	scissorRect.top = 0;
	scissorRect.bottom = 1024 * static_cast<int>(RenderingPara::kShadowMapResolutionScale);
	// Scirssorを設定
	list->RSSetScissorRects(1, &scissorRect);
}

void ShadowMapCommand::PostDraw(ID3D12GraphicsCommandList* list) {
	HRESULT hr = S_FALSE;

	// TransitionBarrierの設定
	D3D12_RESOURCE_BARRIER barrier = MakeResourceBarrier(
		resource_,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		D3D12_RESOURCE_STATE_GENERIC_READ
	);

	// リソースバリアをセット
	list->ResourceBarrier(1, &barrier);

	// コマンドリストの内容を確定させる。全てのコマンドを積んでからcloseすること
	hr = list->Close();
	assert(SUCCEEDED(hr));
}

void ShadowMapCommand::End() {
	// 最後に点光源の数を戻す(÷6)
	structCount_->pointLight /= 6;
}

void ShadowMapCommand::CreatePSO(ID3D12Device* device, DXC* dxc, ID3D12RootSignature* rootSignature) {
	pso_ = std::make_unique<PSO>();
	pso_->InitializeForShadow(device, rootSignature, dxc);
}