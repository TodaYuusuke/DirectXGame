#include "SubRendering.h"
#include "../../descriptorHeap/RTV.h"
#include "../../descriptorHeap/DSV.h"
#include "../../descriptorHeap/SRV.h"

#include <Adapter.h>

using namespace LWP::Base;

void SubRendering::SetDrawTarget(const Math::Matrix4x4& vp, Resource::RenderTexture* renderTexture) {
	renderTexture_ = renderTexture;
	*vpResourceBuffer_->data_ = vp;
}


void SubRendering::PreDraw(ID3D12GraphicsCommandList* list) {
	// 書き込み先のリソースを取得
	RenderResource* rr = renderTexture_->GetRenderResource();

	// リソースバリアをセット
	rr->SetResourceBarrier(D3D12_RESOURCE_STATE_RENDER_TARGET, list);

	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = rtv_->GetCPUHandle(rr->GetRTV());
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = dsv_->GetCPUHandle(rr->GetDSV());
	// 描画先のRTVとDSVを設定する
	list->OMSetRenderTargets(1, &rtvHandle, false, &dsvHandle);

	// 全画面クリア
	rtv_->ClearRenderTarget(rr->GetRTV(), list, { 0.0f,1.0f,0.0f,1.0f });
	// 指定した深度で画面全体をクリアする
	dsv_->ClearDepth(rr->GetDSV(), list);

	// 描画用のSRVのDescriptorHeapを設定
	ID3D12DescriptorHeap* descriptorHeaps[] = { srv_->GetHeap() };
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

void SubRendering::Draw(ID3D12RootSignature* rootSignature, ID3D12GraphicsCommandList* list, ViewStruct viewStruct) {
	// 描画前処理
	PreDraw(list);

	// RootSignatureを設定。PSOに設定してるけど別途設定が必要
	list->SetGraphicsRootSignature(rootSignature);
	// 形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけば良い
	list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// PSOを設定
	list->SetPipelineState(pso_->state_.Get());

	// ディスクリプタテーブルを登録
	list->SetGraphicsRootDescriptorTable(0, indexData_->GetView());
	list->SetGraphicsRootConstantBufferView(1, vpResourceBuffer_->view_);
	list->SetGraphicsRootConstantBufferView(2, viewStruct.commonData);
	list->SetGraphicsRootDescriptorTable(3, viewStruct.vertex);
	list->SetGraphicsRootDescriptorTable(4, viewStruct.wtf);
	list->SetGraphicsRootDescriptorTable(5, viewStruct.material);
	list->SetGraphicsRootDescriptorTable(6, viewStruct.directionLight);
	list->SetGraphicsRootDescriptorTable(7, viewStruct.pointLight);
	list->SetGraphicsRootDescriptorTable(8, viewStruct.texture);
	list->SetGraphicsRootDescriptorTable(9, viewStruct.directionShadowMap);
	list->SetGraphicsRootDescriptorTable(10, viewStruct.pointShadowMap);

	// 全三角形を１つのDrawCallで描画
	list->DrawInstanced(3, indexData_->GetCount() / 3, 0, 0);

	PostDraw(list);
}

void SubRendering::PostDraw(ID3D12GraphicsCommandList* list) {
	// 書き込み先のリソースを取得
	RenderResource* rr = renderTexture_->GetRenderResource();

	// TexResourceのバリアを、コピーされる用に
	D3D12_RESOURCE_BARRIER barrier0 = MakeResourceBarrier(
		renderTexture_->GetTexResource(),
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		D3D12_RESOURCE_STATE_COPY_DEST
	);
	// TexResourceのバリアを、コピーされる用に
	D3D12_RESOURCE_BARRIER barrier1 = MakeResourceBarrier(
		renderTexture_->GetTexResource(),
		D3D12_RESOURCE_STATE_COPY_DEST,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
	);
	
	// レンダリング結果をコピーする
	
	// 書き込み対象をコピーする用のバリアに
	rr->SetResourceBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE, list);
	list->ResourceBarrier(1, &barrier0);
	list->CopyResource(renderTexture_->GetTexResource(), rr->GetResource());
	rr->SetResourceBarrier(D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, list);
	list->ResourceBarrier(1, &barrier1);
}

void SubRendering::CreatePSO(ID3D12Device* device, DXC* dxc, ID3D12RootSignature* rootSignature) {
	pso_ = std::make_unique<PSO>();
	pso_->Initialize(device, rootSignature, dxc,
		{ RasterizerState::CullMode::Back, RasterizerState::FillMode::Solid },
		1, 1, DepthFormat::D24_UNORM_S8_UINT);
}