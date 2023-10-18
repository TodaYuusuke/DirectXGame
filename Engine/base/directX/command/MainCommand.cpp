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

void MainCommand::PreDraw(D3D12_RESOURCE_BARRIER barrier, UINT backBufferIndex, RTV* rtv, DSV* dsv, SRV* srv) {
	// リソースバリアをセット
	list_->ResourceBarrier(1, &barrier);

	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = rtv->GetCPUHandle(backBufferIndex);
	// レンダーターゲットビュー用ディスクリプタヒープのハンドルを取得
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = dsv->GetCPUHandle(0);
	// 描画先のRTVとDSVを設定する
	list_->OMSetRenderTargets(1, &rtvHandle, false, &dsvHandle);

	// 全画面クリア
	rtv->ClearRenderTarget(backBufferIndex, list_.Get());
	// 指定した深度で画面全体をクリアする
	dsv->ClearDepth(list_.Get());

	// 描画用のSRVのDescriptorHeapを設定
	ID3D12DescriptorHeap* descriptorHeaps[] = { srv->GetHeap() };
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
}

void MainCommand::PostDraw(D3D12_RESOURCE_BARRIER barrier) {
	HRESULT hr = S_FALSE;

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