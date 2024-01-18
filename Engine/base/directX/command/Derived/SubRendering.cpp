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
	// TransitionBarrierの設定
	D3D12_RESOURCE_BARRIER barrier = MakeResourceBarrier(
		renderTexture_->GetResource(),
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		D3D12_RESOURCE_STATE_RENDER_TARGET
	);

	// リソースバリアをセット
	list->ResourceBarrier(1, &barrier);

	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = rtv_->GetCPUHandle(renderTexture_->GetRTV());
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = dsv_->GetCPUHandle(renderTexture_->GetDSV());
	// 描画先のRTVとDSVを設定する
	list->OMSetRenderTargets(1, &rtvHandle, false, &dsvHandle);

	// 全画面クリア
	rtv_->ClearRenderTarget(renderTexture_->GetRTV(), list, { 0.0f,1.0f,0.0f,1.0f });
	// 指定した深度で画面全体をクリアする
	dsv_->ClearDepth(renderTexture_->GetDSV(), list);

	// 描画用のSRVのDescriptorHeapを設定
	ID3D12DescriptorHeap* descriptorHeaps[] = { srv_->GetHeap() };
	list->SetDescriptorHeaps(1, descriptorHeaps);

	// ビューポート
	D3D12_VIEWPORT viewport = {};
	// クライアント領域のサイズと一緒にして画面全体に表示
	viewport.Width = static_cast<float>(renderTexture_->kWidth);
	viewport.Height = static_cast<float>(renderTexture_->kHeight);
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
	scissorRect.right = renderTexture_->kWidth;
	scissorRect.top = 0;
	scissorRect.bottom = renderTexture_->kHeight;
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
	// 書き込み対象をコピーする用のバリアに
	D3D12_RESOURCE_BARRIER barrier0 = MakeResourceBarrier(
		renderTexture_->GetResource(),
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_COPY_SOURCE
	);
	// TexResourceのバリアを、コピーされる用に
	D3D12_RESOURCE_BARRIER barrier1 = MakeResourceBarrier(
		renderTexture_->GetTexResource(),
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		D3D12_RESOURCE_STATE_COPY_DEST
	);
	// 書き込み対象を読み取り用のバリアに
	D3D12_RESOURCE_BARRIER barrier2 = MakeResourceBarrier(
		renderTexture_->GetResource(),
		D3D12_RESOURCE_STATE_COPY_SOURCE,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
	);
	// TexResourceのバリアを、コピーされる用に
	D3D12_RESOURCE_BARRIER barrier3 = MakeResourceBarrier(
		renderTexture_->GetTexResource(),
		D3D12_RESOURCE_STATE_COPY_DEST,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
	);
	
	// レンダリング結果をコピーする
	list->ResourceBarrier(1, &barrier0);
	list->ResourceBarrier(1, &barrier1);
	list->CopyResource(renderTexture_->GetTexResource(), renderTexture_->GetResource());
	list->ResourceBarrier(1, &barrier2);
	list->ResourceBarrier(1, &barrier3);
}

void SubRendering::CreatePSO(ID3D12Device* device, DXC* dxc, ID3D12RootSignature* rootSignature) {
	pso_ = std::make_unique<PSO>();
	pso_->Initialize(device, rootSignature, dxc,
		{ RasterizerState::CullMode::Back, RasterizerState::FillMode::Solid },
		1, 1, DepthFormat::D24_UNORM_S8_UINT);
}