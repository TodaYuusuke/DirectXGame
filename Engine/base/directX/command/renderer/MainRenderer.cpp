#include "MainRenderer.h"
#include "object/core/Camera.h"

#include <Adapter.h>

// 今だけResourceを作る関数のためにincludeする
#include "../../resource/structuredBuffer/IStructured.h"
// 今だけResourceBarrierを作る関数のためにincludeする
#include "../ICommand.h"

using namespace LWP::Base;
using namespace LWP::Object;
constexpr int kMaxRendering = lwpC::Rendering::kMaxMultiWindowRendering;	// 最大値を省略

void MainRenderer::Init(ID3D12Device* device, DXC* dxc, HeapManager* heaps) {
	// ルートシグネチャを生成
	root_ = std::make_unique<RootSignature>();
	root_->AddTableParameter(0, SV_All)	// インデックスのデータ
		.AddCBVParameter(0, SV_All)	// 描画に使うViewprojection
		.AddCBVParameter(1, SV_All)	// 全画面で共通のデータ
		.AddTableParameter(1, SV_Vertex)	// 頂点データ
		.AddTableParameter(2, SV_Vertex)	// トランスフォーム
		.AddTableParameter(1, SV_Pixel)	// マテリアル
		.AddTableParameter(2, SV_Pixel)	// 平行光源
		.AddTableParameter(3, SV_Pixel)	// 点光源
		.AddTableParameter(0, SV_Pixel, 1, lwpC::Rendering::kMaxTexture)	// テクスチャ
		.AddTableParameter(0, SV_Pixel, 2, lwpC::Shadow::Direction::kMaxCount)	// 平行光源のシャドウマップ
		.AddTableParameter(0, SV_Pixel, 3, lwpC::Shadow::Point::kMaxCount)	// 点光源のシャドウマップ
		.AddSampler(0, SV_Pixel)	// テクスチャ用サンプラー
		.AddSampler(1, SV_Pixel, D3D12_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT, D3D12_COMPARISON_FUNC_LESS_EQUAL)	// 平行光源のシャドウマップ用サンプラー
		.AddSampler(2, SV_Pixel, D3D12_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT, D3D12_COMPARISON_FUNC_LESS_EQUAL		// 点光源のシャドウマップ用サンプラー
			, D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP)
		.Build(device);
	// PSOを生成
	pso_ = std::make_unique<PSO>();
	pso_->Init(root_->GetRoot(), dxc)
		.SetVertexShader("Object3d.VS.hlsl")
		.SetPixelShader("Object3d.PS.hlsl")
		.Build(device);
	// ヒープ管理クラスをセット
	heaps_ = heaps;

	// ViewProjection用のリソースを生成する
	renderData_ = std::make_unique<RenderData>();
	renderData_->cameraBuffer = std::make_unique<CameraResourceBuffer>();
	renderData_->cameraBuffer->resource_ = LWP::Base::CreateBufferResourceStatic(device, sizeof(CameraStruct));
	renderData_->cameraBuffer->resource_->Map(0, nullptr, reinterpret_cast<void**>(&renderData_->cameraBuffer->data_));
	renderData_->cameraBuffer->view_ = renderData_->cameraBuffer->resource_->GetGPUVirtualAddress();

	// IndexInfoの実態作成
	indexInfo_ = std::make_unique<IStructured<IndexInfoStruct>>(device, heaps_->srv(), lwpC::Rendering::kMaxIndex);
}
void MainRenderer::SetViewStruct(ViewStruct viewStruct) {
	viewStruct_ = viewStruct;
}

void MainRenderer::Draw(ID3D12GraphicsCommandList* list) {
	if(renderData_->target->isUsePostProcess){
		PreDraw(list);
	}
	else {
		PreLastDraw(list);
	}

	// RootSignatureを設定。PSOに設定してるけど別途設定が必要
	list->SetGraphicsRootSignature(root_->GetRoot());
	// 形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけば良い
	list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// PSOを設定
	list->SetPipelineState(pso_->GetState());

	// ディスクリプタテーブルを登録
	list->SetGraphicsRootDescriptorTable(0, indexInfo_->GetView());
	list->SetGraphicsRootConstantBufferView(1, renderData_->cameraBuffer->view_);
	list->SetGraphicsRootConstantBufferView(2, viewStruct_.commonData);
	list->SetGraphicsRootDescriptorTable(3, viewStruct_.vertex);
	list->SetGraphicsRootDescriptorTable(4, viewStruct_.wtf);
	list->SetGraphicsRootDescriptorTable(5, viewStruct_.material);
	list->SetGraphicsRootDescriptorTable(6, viewStruct_.directionLight);
	list->SetGraphicsRootDescriptorTable(7, viewStruct_.pointLight);
	list->SetGraphicsRootDescriptorTable(8, viewStruct_.texture);
	list->SetGraphicsRootDescriptorTable(9, viewStruct_.directionShadowMap);
	list->SetGraphicsRootDescriptorTable(10, viewStruct_.pointShadowMap);

	// 全三角形を１つのDrawCallで描画
	list->DrawInstanced(3, indexInfo_->GetCount() / 3, 0, 0);

	if (renderData_->target->isUsePostProcess) {
		PostDraw(list);
	}
	else {
		PostLastDraw(list);
	}
}
void MainRenderer::Reset() {
	indexInfo_->Reset();
}

void MainRenderer::SetRenderTarget(Camera* camera) {
	// レンダリングターゲットを登録
	renderData_->target = camera;
	*renderData_->cameraBuffer->data_ = *camera;
}
void MainRenderer::AddRenderData(const IndexInfoStruct& indexInfo) {
	indexInfo_->AddData(indexInfo);
}

void MainRenderer::PreDraw(ID3D12GraphicsCommandList* list) {
	// 書き込み先のリソースを取得
	RenderResource* rr = renderData_->target->GetRenderTexture()->GetRenderResource();

	// リソースバリアをセット
	rr->SetResourceBarrier(D3D12_RESOURCE_STATE_RENDER_TARGET, list);
	//rr->SetResourceBarrier(D3D12_RESOURCE_STATE_DEPTH_WRITE, list);

	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = heaps_->rtv()->GetCPUHandle(rr->GetRTV());
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = heaps_->dsv()->GetCPUHandle(rr->GetDSV());
	// 描画先のRTVとDSVを設定する
	list->OMSetRenderTargets(1, &rtvHandle, false, &dsvHandle);

	// 全画面クリア
	heaps_->rtv()->ClearRenderTarget(rr->GetRTV(), list, { 0.0f,1.0f,0.0f,1.0f });
	// 指定した深度で画面全体をクリアする
	heaps_->dsv()->ClearDepth(rr->GetDSV(), list);

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
void MainRenderer::PostDraw(ID3D12GraphicsCommandList* list) {
	// 書き込み先のリソースを取得
	RenderResource* rr = renderData_->target->GetRenderTexture()->GetRenderResource();
	
	// 深度マップを読み取り専用に
	//rr->SetDepthMapResourceBarrier(D3D12_RESOURCE_STATE_GENERIC_READ, list);

	// TexResourceのバリアを、コピーされる用に
	D3D12_RESOURCE_BARRIER barrier0 = ICommand::MakeResourceBarrier(
		renderData_->target->GetRenderTexture()->GetTexResource(),
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		D3D12_RESOURCE_STATE_COPY_DEST
	);
	// TexResourceのバリアを、コピーされる用に
	D3D12_RESOURCE_BARRIER barrier1 = ICommand::MakeResourceBarrier(
		renderData_->target->GetRenderTexture()->GetTexResource(),
		D3D12_RESOURCE_STATE_COPY_DEST,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
	);

	// レンダリング結果をコピーする

	// 書き込み対象をコピーする用のバリアに
	rr->SetResourceBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE, list);
	list->ResourceBarrier(1, &barrier0);
	list->CopyResource(renderData_->target->GetRenderTexture()->GetTexResource(), rr->GetResource());
	rr->SetResourceBarrier(D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, list);
	list->ResourceBarrier(1, &barrier1);

	// 最後にポストプロセスを実行
	renderData_->target->GetPPRenderer()->Draw(list, renderData_->target->GetRenderTexture(), true);
}

// -----  ラストDraw  ----- //

void MainRenderer::PreLastDraw(ID3D12GraphicsCommandList* list) {
	// TransitionBarrierの設定
	D3D12_RESOURCE_BARRIER barrier = ICommand::MakeResourceBarrier(
		heaps_->rtv()->GetBackBuffer(),
		D3D12_RESOURCE_STATE_PRESENT,
		D3D12_RESOURCE_STATE_RENDER_TARGET
	);
	D3D12_RESOURCE_BARRIER depthBarrier = ICommand::MakeResourceBarrier(
		heaps_->dsv()->GetBackBuffersDepth(),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		D3D12_RESOURCE_STATE_DEPTH_WRITE
	);
	// リソースバリアをセット
	list->ResourceBarrier(1, &barrier);
	// リソースバリアをセット
	depthBarrier;
	//list->ResourceBarrier(1, &depthBarrier);

	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = heaps_->rtv()->GetCPUHandle(heaps_->rtv()->GetBackBufferIndex());
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = heaps_->dsv()->GetBackBuffersDepthView();
	// 描画先のRTVとDSVを設定する
	list->OMSetRenderTargets(1, &rtvHandle, false, &dsvHandle);

	// 全画面クリア
	heaps_->rtv()->ClearRenderTarget(heaps_->rtv()->GetBackBufferIndex(), list);
	// 指定した深度で画面全体をクリアする
	heaps_->dsv()->ClearDepth(heaps_->dsv()->GetBackBuffersDepthIndex(), list);

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
void MainRenderer::PostLastDraw(ID3D12GraphicsCommandList* list) {
	// ImGuiの設定をする
	ImGui::Render();
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), list);
	
	// TransitionBarrierの設定
	D3D12_RESOURCE_BARRIER barrier = ICommand::MakeResourceBarrier(
		heaps_->rtv()->GetBackBuffer(),
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_PRESENT
	);
	D3D12_RESOURCE_BARRIER depthBarrier = ICommand::MakeResourceBarrier(
		heaps_->dsv()->GetBackBuffersDepth(),
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		D3D12_RESOURCE_STATE_GENERIC_READ
	);

	// リソースバリアをセット
	list->ResourceBarrier(1, &barrier);
	// リソースバリアをセット
	depthBarrier;
	//list->ResourceBarrier(1, &depthBarrier);
}