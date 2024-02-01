#include "SubRenderer.h"
#include "object/core/Camera.h"

// 今だけResourceを作る関数のためにincludeする
#include "../../resource/structuredBuffer/IStructured.h"
// 今だけResourceBarrierを作る関数のためにincludeする
#include "../ICommand.h"

using namespace LWP::Base;
using namespace LWP::Object;
constexpr int kMaxRendering = lwpC::Rendering::kMaxMultiWindowRendering;	// 最大値を省略

void SubRenderer::Init(ID3D12Device* device, RootSignature* rootPtr, PSO* psoPtr, HeapManager* heaps) {
	rootPtr_ = rootPtr;
	psoPtr_ = psoPtr;
	heaps_ = heaps;

	// ViewProjection用のリソースを生成する
	renderData_ = std::make_unique<RenderData>();
	renderData_->vpResource = std::make_unique<MatrixResourceBuffer>();
	renderData_->vpResource->resource_ = LWP::Base::CreateBufferResourceStatic(device, sizeof(Math::Matrix4x4));
	renderData_->vpResource->resource_->Map(0, nullptr, reinterpret_cast<void**>(&renderData_->vpResource->data_));
	renderData_->vpResource->view_ = renderData_->vpResource->resource_->GetGPUVirtualAddress();
}
void SubRenderer::SetViewStruct(ViewStruct viewStruct, D3D12_GPU_DESCRIPTOR_HANDLE indexInfoView) {
	viewStruct_ = viewStruct;
	indexInfoView_ = indexInfoView;
}
void SubRenderer::Draw(ID3D12GraphicsCommandList* list, int instanceCount) {
	PreDraw(list);

	// RootSignatureを設定。PSOに設定してるけど別途設定が必要
	list->SetGraphicsRootSignature(rootPtr_->GetRoot());
	// 形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけば良い
	list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// PSOを設定
	list->SetPipelineState(psoPtr_->GetState());

	// ディスクリプタテーブルを登録
	list->SetGraphicsRootDescriptorTable(0, indexInfoView_);
	list->SetGraphicsRootConstantBufferView(1, renderData_->vpResource->view_);
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
	list->DrawInstanced(3, instanceCount, 0, 0);

	PostDraw(list);
}

void SubRenderer::SetRenderTarget(Camera* camera) {
	// レンダリングターゲットを登録
	renderData_->target = camera;
	*renderData_->vpResource->data_ = camera->GetViewProjection();
}

void SubRenderer::PreDraw(ID3D12GraphicsCommandList* list) {
	// 書き込み先のリソースを取得
	RenderResource* rr = renderData_->target->GetRenderTexture()->GetRenderResource();

	// リソースバリアをセット
	rr->SetResourceBarrier(D3D12_RESOURCE_STATE_RENDER_TARGET, list);

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
void SubRenderer::PostDraw(ID3D12GraphicsCommandList* list) {
	// 書き込み先のリソースを取得
	RenderResource* rr = renderData_->target->GetRenderTexture()->GetRenderResource();

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
	if (renderData_->target->isUsePostProcess) {
		renderData_->target->GetPPRenderer()->Draw(list, renderData_->target->GetRenderTexture());
	}
}
