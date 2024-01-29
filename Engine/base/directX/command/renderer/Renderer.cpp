#include "Renderer.h"

// 今だけResourceを作る関数のためにincludeする
#include "../../resource/structuredBuffer/IStructured.h"
// 今だけResourceBarrierを作る関数のためにincludeする
#include "../ICommand.h"

using namespace LWP::Base;
constexpr int kMaxRendering = lwpC::Rendering::kMaxMultiWindowRendering;	// 最大値を省略

void Renderer::Init(ID3D12Device* device, DXC* dxc, HeapManager* heaps) {
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

	// メインのリソースを生成する
	mainRenderData_ = std::make_unique<MainRenderData>();
	mainRenderData_->indexInfo = std::make_unique<IStructured<IndexInfoStruct>>(device, heaps_->srv(), lwpC::Rendering::kMaxIndex);
	mainRenderData_->renderData = std::make_unique<RenderData>();
	mainRenderData_->renderData->vpResource = std::make_unique<MatrixResourceBuffer>();
	mainRenderData_->renderData->vpResource->resource_ = LWP::Base::CreateBufferResourceStatic(device, sizeof(Math::Matrix4x4));
	mainRenderData_->renderData->vpResource->resource_->Map(0, nullptr, reinterpret_cast<void**>(mainRenderData_->renderData->vpResource->data_));
	mainRenderData_->renderData->vpResource->view_ = mainRenderData_->renderData->vpResource->resource_->GetGPUVirtualAddress();

	// サブのリソースを生成する
	subRenderData_ = std::make_unique<SubRenderData>();
	subRenderData_->indexInfo = std::make_unique<IStructured<IndexInfoStruct>>(device, heaps_->srv(), lwpC::Rendering::kMaxIndex);
	for (int i = 0; i < kMaxRendering; i++) {
		subRenderData_->renderData[i]->vpResource = std::make_unique<MatrixResourceBuffer>();
		subRenderData_->renderData[i]->vpResource->resource_ = LWP::Base::CreateBufferResourceStatic(device, sizeof(Math::Matrix4x4));
		subRenderData_->renderData[i]->vpResource->resource_->Map(0, nullptr, reinterpret_cast<void**>(subRenderData_->renderData[i]->vpResource->data_));
		subRenderData_->renderData[i]->vpResource->view_ = subRenderData_->renderData[i]->vpResource->resource_->GetGPUVirtualAddress();
	}
}
void Renderer::MainDraw(ID3D12GraphicsCommandList* list, int instanceCount) {

	

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
		list->SetGraphicsRootDescriptorTable(1, heaps_->srv()->GetGPUHandle(renderData_[i]->target->GetTexSRVIndex()));

		// 四角形を描画
		list->DrawInstanced(3, 2, 0, 0);

		// メインレンダリングのときのみ関数を変える
		if(renderData_[i]->isMain) {
			PostMainDraw(list, i);
		}
		else {
			PostDraw(list, i);
		}
	}
}
void Renderer::Reset() {
	// 使用数を元に戻す
	counter_.Reset();
}

void Renderer::SetSubRenderData(Resource::RenderTexture* target, Math::Matrix4x4 vp) {
	// オフならば何もしない
	if (!data.isActive) { return; }

	// レンダリング用のデータを登録
	renderData_[counter_.Get()]->target = target;
	*renderData_[counter_.Get()]->data = data;
	renderData_[counter_.Get()]->isMain = isMain;

	// カウンターを増やして終了
	counter_.GetAndIncrement();
}

void Renderer::PreDraw(ID3D12GraphicsCommandList* list, int index) {
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
void Renderer::PostDraw(ID3D12GraphicsCommandList* list, int index) {
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

void Renderer::PostMainDraw(ID3D12GraphicsCommandList* list, int index) {
	// 書き込み先のリソースを取得
	RenderResource* rr = renderData_[index]->target->GetRenderResource();
	// メインレンダリングのときはバックバッファにコピーする
	ID3D12Resource* backBuffer = heaps_->rtv()->GetBackBuffer();

	// バックバッファのバリアを、コピーされる用に
	D3D12_RESOURCE_BARRIER barrier0 = ICommand::MakeResourceBarrier(
		backBuffer,
		D3D12_RESOURCE_STATE_PRESENT,
		D3D12_RESOURCE_STATE_COPY_DEST
	);
	// バックバッファのバリアを、コピーされる用に
	D3D12_RESOURCE_BARRIER barrier1 = ICommand::MakeResourceBarrier(
		backBuffer,
		D3D12_RESOURCE_STATE_COPY_DEST,
		D3D12_RESOURCE_STATE_PRESENT
	);

	// レンダリング結果をコピーする

	// 書き込み対象をコピーする用のバリアに
	rr->SetResourceBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE, list);
	list->ResourceBarrier(1, &barrier0);
	list->CopyResource(backBuffer, rr->GetResource());
	rr->SetResourceBarrier(D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, list);
	list->ResourceBarrier(1, &barrier1);
}
