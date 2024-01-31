#include "Renderer.h"
#include "object/core/Camera.h"

// 今だけResourceを作る関数のためにincludeする
#include "../../resource/structuredBuffer/IStructured.h"
// 今だけResourceBarrierを作る関数のためにincludeする
#include "../ICommand.h"

using namespace LWP::Base;
constexpr int kMaxRendering = lwpC::Rendering::kMaxMultiWindowRendering;	// 最大値を省略

void Renderer::Init(ID3D12Device* device, DXC* dxc, HeapManager* heaps) {
	// ポインタをセット
	device_ = device;
	dxc_ = dxc;
	// ヒープ管理クラスをセット
	heaps_ = heaps;

	// メインレンダリングの実態を生成する
	mainRenderer_ = std::make_unique<MainRenderer>();
	mainRenderer_->Init(device, dxc, heaps);

	// ポストプロセス用のRootSignatureを生成
	ppRoot_ = std::make_unique<RootSignature>();
	ppRoot_->AddCBVParameter(0, SV_Pixel)	// レンダリング用のデータ
		.AddTableParameter(0, SV_Pixel)	// レンダリングに使うテクスチャ
		.AddSampler(0, SV_Pixel)	// テクスチャのサンプラー
		.Build(device);

	// サブのリソースを生成する
	//subRenderData_ = std::make_unique<SubRenderData>();
	//subRenderData_->indexInfo = std::make_unique<IStructured<IndexInfoStruct>>(device, heaps_->srv(), lwpC::Rendering::kMaxIndex);
	//for (int i = 0; i < kMaxRendering; i++) {
	//	subRenderData_->renderData[i]->vpResource = std::make_unique<MatrixResourceBuffer>();
	//	subRenderData_->renderData[i]->vpResource->resource_ = LWP::Base::CreateBufferResourceStatic(device, sizeof(Math::Matrix4x4));
	//	subRenderData_->renderData[i]->vpResource->resource_->Map(0, nullptr, reinterpret_cast<void**>(subRenderData_->renderData[i]->vpResource->data_));
	//	subRenderData_->renderData[i]->vpResource->view_ = subRenderData_->renderData[i]->vpResource->resource_->GetGPUVirtualAddress();
	//}
}
void Renderer::SetViewStruct(ViewStruct viewStruct) {
	mainRenderer_->SetViewStruct(viewStruct);
}
void Renderer::Draw(ID3D12GraphicsCommandList* list) {
	HRESULT hr = S_FALSE;

	mainRenderer_->Draw(list);

	// コマンドリストの内容を確定させる。全てのコマンドを積んでからcloseすること
	hr = list->Close();
	assert(SUCCEEDED(hr));
}
void Renderer::Reset() {
	mainRenderer_->Reset();
}

void Renderer::SetMainRenderTarget(LWP::Object::Camera* camera) {
	mainRenderer_->SetRenderTarget(camera);

	// ポストプロセスをする予定で、ポストプロセス用のコマンドクラスを持っていない場合作成する
	if (camera->isUsePostProcess && !camera->GetPPRenderer()) {
		PostProcessRenderer* newRenderer = new PostProcessRenderer();
		newRenderer->Init(device_, dxc_, heaps_, ppRoot_.get(), camera->shaderPath);
		camera->SetPPRenderer(newRenderer);
	}
}
void Renderer::AddMainRenderData(const IndexInfoStruct& indexInfo) {
	mainRenderer_->AddRenderData(indexInfo);
}

//void Renderer::SetSubRenderData(Resource::RenderTexture* target, Math::Matrix4x4 vp) {
//	// オフならば何もしない
//	if (!data.isActive) { return; }
//
//	// レンダリング用のデータを登録
//	renderData_[counter_.Get()]->target = target;
//	*renderData_[counter_.Get()]->data = data;
//	renderData_[counter_.Get()]->isMain = isMain;
//
//	// カウンターを増やして終了
//	counter_.GetAndIncrement();
//}
//
//void Renderer::PreDraw(ID3D12GraphicsCommandList* list, int index) {
//	// 書き込み先のリソースを取得
//	RenderResource* rr = renderData_[index]->target->GetRenderResource();
//
//	// リソースバリアをセット
//	rr->SetResourceBarrier(D3D12_RESOURCE_STATE_RENDER_TARGET, list);
//
//	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = heaps_->rtv()->GetCPUHandle(rr->GetRTV());
//	//D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = heaps_->dsv()->GetCPUHandle(rr->GetDSV());
//	// 描画先のRTVとDSVを設定する
//	list->OMSetRenderTargets(1, &rtvHandle, false, nullptr);
//
//	// 全画面クリア
//	heaps_->rtv()->ClearRenderTarget(rr->GetRTV(), list, { 0.0f,1.0f,0.0f,1.0f });
//	// 指定した深度で画面全体をクリアする
//	//heaps_->dsv()->ClearDepth(rr->GetDSV(), list);
//
//	// 描画用のSRVのDescriptorHeapを設定
//	ID3D12DescriptorHeap* descriptorHeaps[] = { heaps_->srv()->GetHeap() };
//	list->SetDescriptorHeaps(1, descriptorHeaps);
//
//	// ビューポート
//	D3D12_VIEWPORT viewport = {};
//	// クライアント領域のサイズと一緒にして画面全体に表示
//	viewport.Width = rr->GetResolution().x;
//	viewport.Height = rr->GetResolution().y;
//	viewport.TopLeftX = 0;
//	viewport.TopLeftY = 0;
//	viewport.MinDepth = 0.0f;
//	viewport.MaxDepth = 1.0f;
//	// viewportを設定
//	list->RSSetViewports(1, &viewport);
//
//	// シザー矩形
//	D3D12_RECT scissorRect = {};
//	// 基本的にビューポートと同じ矩形が構成されるようにする
//	scissorRect.left = 0;
//	scissorRect.right = static_cast<LONG>(rr->GetResolution().x);
//	scissorRect.top = 0;
//	scissorRect.bottom = static_cast<LONG>(rr->GetResolution().y);
//	// Scirssorを設定
//	list->RSSetScissorRects(1, &scissorRect);
//}
//void Renderer::PostDraw(ID3D12GraphicsCommandList* list, int index) {
//	// 書き込み先のリソースを取得
//	RenderResource* rr = renderData_[index]->target->GetRenderResource();
//
//	// TexResourceのバリアを、コピーされる用に
//	D3D12_RESOURCE_BARRIER barrier0 = ICommand::MakeResourceBarrier(
//		renderData_[index]->target->GetTexResource(),
//		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
//		D3D12_RESOURCE_STATE_COPY_DEST
//	);
//	// TexResourceのバリアを、コピーされる用に
//	D3D12_RESOURCE_BARRIER barrier1 = ICommand::MakeResourceBarrier(
//		renderData_[index]->target->GetTexResource(),
//		D3D12_RESOURCE_STATE_COPY_DEST,
//		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
//	);
//
//	// レンダリング結果をコピーする
//
//	// 書き込み対象をコピーする用のバリアに
//	rr->SetResourceBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE, list);
//	list->ResourceBarrier(1, &barrier0);
//	list->CopyResource(renderData_[index]->target->GetTexResource(), rr->GetResource());
//	rr->SetResourceBarrier(D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, list);
//	list->ResourceBarrier(1, &barrier1);
//}
//
//void Renderer::PostMainDraw(ID3D12GraphicsCommandList* list, int index) {
//	// 書き込み先のリソースを取得
//	RenderResource* rr = renderData_[index]->target->GetRenderResource();
//	// メインレンダリングのときはバックバッファにコピーする
//	ID3D12Resource* backBuffer = heaps_->rtv()->GetBackBuffer();
//
//	// バックバッファのバリアを、コピーされる用に
//	D3D12_RESOURCE_BARRIER barrier0 = ICommand::MakeResourceBarrier(
//		backBuffer,
//		D3D12_RESOURCE_STATE_PRESENT,
//		D3D12_RESOURCE_STATE_COPY_DEST
//	);
//	// バックバッファのバリアを、コピーされる用に
//	D3D12_RESOURCE_BARRIER barrier1 = ICommand::MakeResourceBarrier(
//		backBuffer,
//		D3D12_RESOURCE_STATE_COPY_DEST,
//		D3D12_RESOURCE_STATE_PRESENT
//	);
//
//	// レンダリング結果をコピーする
//
//	// 書き込み対象をコピーする用のバリアに
//	rr->SetResourceBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE, list);
//	list->ResourceBarrier(1, &barrier0);
//	list->CopyResource(backBuffer, rr->GetResource());
//	rr->SetResourceBarrier(D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, list);
//	list->ResourceBarrier(1, &barrier1);
//}
