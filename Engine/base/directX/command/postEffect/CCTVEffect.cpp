#include "CCTVEffect.h"

// 今だけResourceを作る関数のためにincludeする
#include "../../resource/structuredBuffer/IStructured.h"
// 今だけResourceBarrierを作る関数のためにincludeする
#include "../ICommand.h"

using namespace LWP::Base;
using namespace LWP::Base::PostProcess;
constexpr int kMax = LWP::Config::PostProcess::kMaxLensDistortion;	// 最大値を省略

void CCTVEffectRenderer::Init(ID3D12Device* device, DXC* dxc, HeapManager* heaps) {
	// ルートシグネチャを生成
	root_ = std::make_unique<RootSignature>();
	root_->AddCBVParameter(0, SV_Pixel)	// レンダリング用のデータ
		.AddTableParameter(0, SV_Pixel)	// レンダリングに使うテクスチャ
		.AddSampler(0, SV_Pixel)	// テクスチャのサンプラー
		.Build(device);
	// PSOを生成
	pso_ = std::make_unique<PSO>();
	pso_->Init(root_->GetRoot(), dxc)
		.SetDepthStencilState(false)
		.SetVertexShader("postProcess/PassThrough.VS.hlsl")
		.SetPixelShader("postProcess/CCTV.PS.hlsl")
		.Build(device);
	// ヒープ管理クラスをセット
	heaps_ = heaps;

	// リソースを生成
	for (int i = 0; i < kMax; i++) {
		renderData_[i] = std::make_unique<RenderData>();
		renderData_[i]->resource = CreateBufferResourceStatic(device, sizeof(CCTVStruct));
		renderData_[i]->resource->Map(0, nullptr, reinterpret_cast<void**>(&renderData_[i]->data));
		renderData_[i]->view = renderData_[i]->resource->GetGPUVirtualAddress();
	}
}
void CCTVEffectRenderer::Draw(ID3D12GraphicsCommandList* list) {
	for (int i = 0; i < counter_.Get(); i++) {
		PreDraw(list, i);

		// RootSignatureを設定。PSOに設定してるけど別途設定が必要
		list->SetGraphicsRootSignature(root_->GetRoot());
		// 形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけば良い
		list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		// PSOを設定
		list->SetPipelineState(pso_->GetState());

		// ディスクリプタテーブルを登録
		list->SetGraphicsRootConstantBufferView(0, renderData_[i]->view);
		//list->SetGraphicsRootDescriptorTable(1, heaps_->srv()->GetGPUHandle(renderData_[i]->target->GetIndex()));
		list->SetGraphicsRootDescriptorTable(1, heaps_->srv()->GetGPUHandle(74));

		// 四角形を描画
		list->DrawInstanced(3, 2, 0, 0);


		PostDraw(list, i);
	}
}
void CCTVEffectRenderer::Reset() {
	// 使用数を元に戻す
	counter_.Reset();
}

void CCTVEffectRenderer::SetRenderData(Resource::RenderTexture* target, CCTVEffect data) {
	// オフならば何もしない
	if (!data.isActive) { return; }

	// レンダリング用のデータを登録
	renderData_[counter_.Get()]->target = target;
	*renderData_[counter_.Get()]->data = data;

	// カウンターを増やして終了
	counter_.GetAndIncrement();
}

void CCTVEffectRenderer::PreDraw(ID3D12GraphicsCommandList* list, int index) {
	// 書き込み先のリソースを取得
	RenderResource* rr = renderData_[index]->target->GetRenderResource();

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
void CCTVEffectRenderer::PostDraw(ID3D12GraphicsCommandList* list, int index) {
	// 書き込み先のリソースを取得
	RenderResource* rr = renderData_[index]->target->GetRenderResource();

	// TexResourceのバリアを、コピーされる用に
	D3D12_RESOURCE_BARRIER barrier0 = ICommand::MakeResourceBarrier(
		renderData_[index]->target->GetTexResource(),
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		D3D12_RESOURCE_STATE_COPY_DEST
	);
	// TexResourceのバリアを、コピーされる用に
	D3D12_RESOURCE_BARRIER barrier1 = ICommand::MakeResourceBarrier(
		renderData_[index]->target->GetTexResource(),
		D3D12_RESOURCE_STATE_COPY_DEST,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
	);

	// レンダリング結果をコピーする

	// 書き込み対象をコピーする用のバリアに
	rr->SetResourceBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE, list);
	list->ResourceBarrier(1, &barrier0);
	list->CopyResource(renderData_[index]->target->GetTexResource(), rr->GetResource());
	rr->SetResourceBarrier(D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, list);
	list->ResourceBarrier(1, &barrier1);
}
