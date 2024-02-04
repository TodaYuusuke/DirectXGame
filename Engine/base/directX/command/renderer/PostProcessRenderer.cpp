#include "PostProcessRenderer.h"

#include <Adapter.h>

// 今だけResourceを作る関数のためにincludeする
#include "../../resource/structuredBuffer/IStructured.h"
// 今だけResourceBarrierを作る関数のためにincludeする
#include "../ICommand.h"

using namespace LWP::Base;

// 初期化処理
void PostProcessRenderer::Init(ID3D12Device* device, DXC* dxc, HeapManager* heaps, RootSignature* root, std::string path) {
	// ルートシグネチャをセット
	rootPtr_ = root;
	// PSOを生成
	pso_ = std::make_unique<PSO>();
	pso_->Init(rootPtr_->GetRoot(), dxc)
		.SetDepthStencilState(false)
		.SetVertexShader("postProcess/PassThrough.VS.hlsl")
		.SetPixelShader(path)
		.Build(device);

	// ポインタ達をセット
	heaps_ = heaps;

	// リソースを生成
	renderData_ = std::make_unique<RenderData>();
	renderData_->resource = CreateBufferResourceStatic(device, sizeof(int));
	renderData_->resource->Map(0, nullptr, reinterpret_cast<void**>(&renderData_->time));
	renderData_->view = renderData_->resource->GetGPUVirtualAddress();
}
// レンダリング
void PostProcessRenderer::Draw(ID3D12GraphicsCommandList* list, Resource::RenderTexture* target, bool isMain) {
	// 経過時間を追加
	*renderData_->time += 1;

	// 深度マップのステータス変更
	//RenderResource* rr = target->GetRenderResource();
	//rr->SetDepthMapResourceBarrier(D3D12_RESOURCE_STATE_S, list);

	if (isMain) {
		PreLastDraw(list);
	}
	else {
		PreDraw(list, target);
	}

	// RootSignatureを設定。PSOに設定してるけど別途設定が必要
	list->SetGraphicsRootSignature(rootPtr_->GetRoot());
	// 形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけば良い
	list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// PSOを設定
	list->SetPipelineState(pso_->GetState());

	// ディスクリプタテーブルを登録
	list->SetGraphicsRootConstantBufferView(0, renderData_->view);
	list->SetGraphicsRootDescriptorTable(1, heaps_->srv()->GetGPUHandle(target->GetTexSRVIndex()));
	//list->SetGraphicsRootDescriptorTable(2, heaps_->srv()->GetGPUHandle(target->GetDepthMapSRVIndex()));

	// 四角形を描画
	list->DrawInstanced(3, 2, 0, 0);

	// 深度マップのステータスを戻す
	//rr->SetDepthMapResourceBarrier(D3D12_RESOURCE_STATE_DEPTH_WRITE, list);
	if (isMain) {
		PostLastDraw(list);
	}
	else {
		PostDraw(list, target);
	}
}

// シェーダー作り直し
void PostProcessRenderer::ReCreateShader(ID3D12Device* device, DXC* dxc, std::string path) {
	// PSOを生成
	pso_.reset(new PSO());
	pso_->Init(rootPtr_->GetRoot(), dxc)
		.SetDepthStencilState(false)
		.SetVertexShader("postProcess/PassThrough.VS.hlsl")
		.SetPixelShader(path)
		.Build(device);
}


void PostProcessRenderer::PreDraw(ID3D12GraphicsCommandList* list, Resource::RenderTexture* target) {
	// 書き込み先のリソースを取得
	RenderResource* rr = target->GetRenderResource();

	// リソースバリアをセット
	rr->SetResourceBarrier(D3D12_RESOURCE_STATE_RENDER_TARGET, list);

	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = heaps_->rtv()->GetCPUHandle(rr->GetRTV());
	//D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = heaps_->dsv()->GetCPUHandle(rr->GetDSV());
	// 描画先のRTVとDSVを設定する
	list->OMSetRenderTargets(1, &rtvHandle, false, nullptr);

	// 全画面クリア
	heaps_->rtv()->ClearRenderTarget(rr->GetRTV(), list, { 0.0f,1.0f,0.0f,1.0f });
	// 指定した深度で画面全体をクリアする
	//heaps_->dsv()->ClearDepth(rr->GetDSV(), list);

	// 描画用のSRVのDescriptorHeapを設定
	ID3D12DescriptorHeap* descriptorHeaps[] = { heaps_->srv()->GetHeap() };
	list->SetDescriptorHeaps(1, descriptorHeaps);

	// ビューポート
	D3D12_VIEWPORT viewport = {};
	// クライアント領域のサイズと一緒にして画面全体に表示
	viewport.Width = rr->GetResolution().x;
	viewport.Height = rr->GetResolution().y;
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
	scissorRect.right = static_cast<LONG>(rr->GetResolution().x);
	scissorRect.top = 0;
	scissorRect.bottom = static_cast<LONG>(rr->GetResolution().y);
	// Scirssorを設定
	list->RSSetScissorRects(1, &scissorRect);
}
void PostProcessRenderer::PostDraw(ID3D12GraphicsCommandList* list, Resource::RenderTexture* target) {
	// 書き込み先のリソースを取得
	RenderResource* rr = target->GetRenderResource();

	// TexResourceのバリアを、コピーされる用に
	D3D12_RESOURCE_BARRIER barrier0 = ICommand::MakeResourceBarrier(
		target->GetTexResource(),
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		D3D12_RESOURCE_STATE_COPY_DEST
	);
	// TexResourceのバリアを、コピーされる用に
	D3D12_RESOURCE_BARRIER barrier1 = ICommand::MakeResourceBarrier(
		target->GetTexResource(),
		D3D12_RESOURCE_STATE_COPY_DEST,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
	);

	// レンダリング結果をコピーする

	// 書き込み対象をコピーする用のバリアに
	rr->SetResourceBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE, list);
	list->ResourceBarrier(1, &barrier0);
	list->CopyResource(target->GetTexResource(), rr->GetResource());
	rr->SetResourceBarrier(D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, list);
	list->ResourceBarrier(1, &barrier1);
}

void PostProcessRenderer::PreLastDraw(ID3D12GraphicsCommandList* list) {
	// TransitionBarrierの設定
	D3D12_RESOURCE_BARRIER barrier = ICommand::MakeResourceBarrier(
		heaps_->rtv()->GetBackBuffer(),
		D3D12_RESOURCE_STATE_PRESENT,
		D3D12_RESOURCE_STATE_RENDER_TARGET
	);
	// リソースバリアをセット
	list->ResourceBarrier(1, &barrier);

	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = heaps_->rtv()->GetCPUHandle(heaps_->rtv()->GetBackBufferIndex());
	//D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = heaps_->dsv()->GetCPUHandle(rr->GetDSV());
	// 描画先のRTVとDSVを設定する
	list->OMSetRenderTargets(1, &rtvHandle, false, nullptr);

	// 全画面クリア
	heaps_->rtv()->ClearRenderTarget(heaps_->rtv()->GetBackBufferIndex(), list);

	// 描画用のSRVのDescriptorHeapを設定
	ID3D12DescriptorHeap* descriptorHeaps[] = { heaps_->srv()->GetHeap() };
	list->SetDescriptorHeaps(1, descriptorHeaps);

	// ビューポート
	D3D12_VIEWPORT viewport = {};
	// クライアント領域のサイズと一緒にして画面全体に表示
	viewport.Width = LWP::Info::GetWindowWidthF();
	viewport.Height = LWP::Info::GetWindowHeightF();
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
void PostProcessRenderer::PostLastDraw(ID3D12GraphicsCommandList* list) {
	// ラストなのでImGuiを入れておく
	ImGui::Render();
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), list);

	// TransitionBarrierの設定
	D3D12_RESOURCE_BARRIER barrier = ICommand::MakeResourceBarrier(
		heaps_->rtv()->GetBackBuffer(),
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_PRESENT
	);

	// リソースバリアをセット
	list->ResourceBarrier(1, &barrier);
}