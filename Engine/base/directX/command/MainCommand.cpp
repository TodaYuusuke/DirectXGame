#include "MainCommand.h"
#include "../descriptorHeap/RTV.h"
#include "../descriptorHeap/DSV.h"
#include "../descriptorHeap/SRV.h"

#include <Adapter.h>

using namespace LWP::Base;


void MainCommand::Initialize(ID3D12Device* device) {
	HRESULT hr = S_FALSE;

	// コマンドアロケーターを生成する
	hr = device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&allocator_));
	// コマンドアロケーターの生成がうまくいかなかったので起動できない
	assert(SUCCEEDED(hr));

	// コマンドリストを生成する
	hr = device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, allocator_.Get(), nullptr, IID_PPV_ARGS(&list_));
	// コマンドリストの生成がうまくいかなかったので起動できない
	assert(SUCCEEDED(hr));
}

void MainCommand::PreDraw(ID3D12RootSignature* rootSignature) {
	// TransitionBarrierの設定
	D3D12_RESOURCE_BARRIER barrier = MakeResourceBarrier(
		rtv_->GetBackBuffer(),
		D3D12_RESOURCE_STATE_PRESENT,
		D3D12_RESOURCE_STATE_RENDER_TARGET
	);

	// リソースバリアをセット
	list_->ResourceBarrier(1, &barrier);

	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = rtv_->GetCPUHandle(rtv_->GetBackBufferIndex());
	// レンダーターゲットビュー用ディスクリプタヒープのハンドルを取得
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = dsv_->GetCPUHandle(0);
	// 描画先のRTVとDSVを設定する
	list_->OMSetRenderTargets(1, &rtvHandle, false, &dsvHandle);

	// 全画面クリア
	rtv_->ClearRenderTarget(list_.Get());
	// 指定した深度で画面全体をクリアする
	dsv_->ClearDepth(0, list_.Get());

	// 描画用のSRVのDescriptorHeapを設定
	ID3D12DescriptorHeap* descriptorHeaps[] = { srv_->GetHeap() };
	list_->SetDescriptorHeaps(1, descriptorHeaps);

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
	list_->RSSetViewports(1, &viewport);

	// シザー矩形
	D3D12_RECT scissorRect = {};
	// 基本的にビューポートと同じ矩形が構成されるようにする
	scissorRect.left = 0;
	scissorRect.right = LWP::Info::GetWindowWidth();
	scissorRect.top = 0;
	scissorRect.bottom = LWP::Info::GetWindowHeight();
	// Scirssorを設定
	list_->RSSetScissorRects(1, &scissorRect);

	// RootSignatureを設定。PSOに設定してるけど別途設定が必要
	list_->SetGraphicsRootSignature(rootSignature);
	// 形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけば良い
	list_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void MainCommand::PostDraw() {
	HRESULT hr = S_FALSE;

	// TransitionBarrierの設定
	D3D12_RESOURCE_BARRIER barrier = MakeResourceBarrier(
		rtv_->GetBackBuffer(),
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_PRESENT
	);

	// リソースバリアをセット
	list_->ResourceBarrier(1, &barrier);

	// コマンドリストの内容を確定させる。全てのコマンドを積んでからcloseすること
	hr = list_->Close();
	assert(SUCCEEDED(hr));
}

void MainCommand::Reset() {
	HRESULT hr = S_FALSE;

	// 次のフレーム用のコマンドリストを準備
	hr = allocator_->Reset();
	assert(SUCCEEDED(hr));
	hr = list_->Reset(allocator_.Get(), nullptr);
	assert(SUCCEEDED(hr));
}