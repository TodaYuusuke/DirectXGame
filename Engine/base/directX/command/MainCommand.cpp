#include "MainCommand.h"
#include "../descriptorHeap/RTV.h"
#include "../descriptorHeap/DSV.h"
#include "../descriptorHeap/SRV.h"

#include <Adapter.h>

using namespace LWP::Base;

void MainCommand::DerivedInitialize() {
	//深度マップを作成
	depthStencil_ = std::make_unique<DepthStencil>();
	dsv_->CreateDepthStencil(depthStencil_->resource_.Get(), &depthStencil_->view_, LWP::Info::GetWindowWidth(), LWP::Info::GetWindowHeight());
};

void MainCommand::PreDraw(ID3D12GraphicsCommandList* list) {
	// TransitionBarrierの設定
	D3D12_RESOURCE_BARRIER barrier = MakeResourceBarrier(
		rtv_->GetBackBuffer(),
		D3D12_RESOURCE_STATE_PRESENT,
		D3D12_RESOURCE_STATE_RENDER_TARGET
	);

	// リソースバリアをセット
	list->ResourceBarrier(1, &barrier);

	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = rtv_->GetCPUHandle(rtv_->GetBackBufferIndex());
	// 描画先のRTVとDSVを設定する
	list->OMSetRenderTargets(1, &rtvHandle, false, &depthStencil_->view_);

	// 全画面クリア
	rtv_->ClearRenderTarget(list);
	// 指定した深度で画面全体をクリアする
	dsv_->ClearDepth(0, list);

	// 描画用のSRVのDescriptorHeapを設定
	ID3D12DescriptorHeap* descriptorHeaps[] = { srv_->GetHeap() };
	list->SetDescriptorHeaps(1, descriptorHeaps);

	// ビューポート
	D3D12_VIEWPORT viewport = {};
	// クライアント領域のサイズと一緒にして画面全体に表示
	viewport.Width = static_cast<float>(LWP::Info::GetWindowWidth());
	viewport.Height = static_cast<float>(LWP::Info::GetWindowHeight());
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
	scissorRect.right = LWP::Info::GetWindowWidth();
	scissorRect.top = 0;
	scissorRect.bottom = LWP::Info::GetWindowHeight();
	// Scirssorを設定
	list->RSSetScissorRects(1, &scissorRect);
}

void MainCommand::PostDraw(ID3D12GraphicsCommandList* list) {
	HRESULT hr = S_FALSE;

	// ImGuiの設定をする
	ImGui::Render();
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), list);

	// TransitionBarrierの設定
	D3D12_RESOURCE_BARRIER barrier = MakeResourceBarrier(
		rtv_->GetBackBuffer(),
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_PRESENT
	);

	// リソースバリアをセット
	list->ResourceBarrier(1, &barrier);

	// コマンドリストの内容を確定させる。全てのコマンドを積んでからcloseすること
	hr = list->Close();
	assert(SUCCEEDED(hr));

}

void MainCommand::CreatePSO(ID3D12Device* device, DXC* dxc, ID3D12RootSignature* rootSignature) {
	pso_ = std::make_unique<PSO>();
	pso_->Initialize(device, rootSignature, dxc,
		{ RasterizerState::CullMode::Back, RasterizerState::FillMode::Solid },
		1, 1, DepthFormat::D24_UNORM_S8_UINT);
}