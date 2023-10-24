#include "ShadowMapCommand.h"
#include "../descriptorHeap/RTV.h"
#include "../descriptorHeap/DSV.h"
#include "../descriptorHeap/SRV.h"

#include <Adapter.h>

using namespace LWP::Base;


void ShadowMapCommand::Initialize(ID3D12Device* device) {
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

void ShadowMapCommand::PreDraw(ID3D12RootSignature* rootSignature) {
	// TransitionBarrierの設定
	D3D12_RESOURCE_BARRIER barrier = MakeResourceBarrier(
		dsv_->GetShadowMapResource(),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		D3D12_RESOURCE_STATE_DEPTH_WRITE
	);

	// リソースバリアをセット
	list_->ResourceBarrier(1, &barrier);

	// シャドウマップ用のDSVハンドルを取得
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = dsv_->GetCPUHandle(1);
	// 描画先のDSVを設定する
	list_->OMSetRenderTargets(0, nullptr, false, &dsvHandle);

	// 指定した深度で画面全体をクリアする（1はシャドウマップ用DSV）
	dsv_->ClearDepth(1, list_.Get());


	// ビューポート
	D3D12_VIEWPORT viewport = {};
	// シャドウマップ用のテクスチャと同じサイズにする
	viewport.Width = 1024.0f;
	viewport.Height = 1024.0f;
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
	scissorRect.right = 1024;
	scissorRect.top = 0;
	scissorRect.bottom = 1024;
	// Scirssorを設定
	list_->RSSetScissorRects(1, &scissorRect);

	// RootSignatureを設定。PSOに設定してるけど別途設定が必要
	list_->SetGraphicsRootSignature(rootSignature);
	// 形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけば良い
	list_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void ShadowMapCommand::PostDraw() {
	HRESULT hr = S_FALSE;

	// TransitionBarrierの設定
	D3D12_RESOURCE_BARRIER barrier = MakeResourceBarrier(
		dsv_->GetShadowMapResource(),
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		D3D12_RESOURCE_STATE_GENERIC_READ
	);

	// リソースバリアをセット
	list_->ResourceBarrier(1, &barrier);

	// コマンドリストの内容を確定させる。全てのコマンドを積んでからcloseすること
	hr = list_->Close();
	assert(SUCCEEDED(hr));
}

void ShadowMapCommand::Reset() {
	HRESULT hr = S_FALSE;

	// 次のフレーム用のコマンドリストを準備
	hr = allocator_->Reset();
	assert(SUCCEEDED(hr));
	hr = list_->Reset(allocator_.Get(), nullptr);
	assert(SUCCEEDED(hr));
}