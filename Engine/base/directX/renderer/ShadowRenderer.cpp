#include "ShadowRenderer.h"

#include "Config.h"

using namespace LWP::Base;

// サイズをここで指定
ShadowRenderer::ShadowRenderer() : indexBuffer_(lwpC::Rendering::kMaxIndex) {}

void ShadowRenderer::Init(GPUDevice* device, SRV* srv, DXC* dxc, std::function<void()> func) {
	// StructuredBufferを初期化
	indexBuffer_.Init(device, srv);
	// RootSignatureを生成
	root_.AddCBVParameter(0, SV_Vertex)	// インデックスのデータ
		.AddTableParameter(0, SV_Vertex)	// 描画に使うViewprojection
		.AddTableParameter(1, SV_Vertex)	// 頂点データ
		.AddTableParameter(2, SV_Vertex)	// トランスフォーム]
		.Build(device->GetDevice());

	// PSOを生成
	pso_.Init(root_, dxc)
		.SetVertexShader("ShadowMap.VS.hlsl")
		.SetRasterizerState(D3D12_CULL_MODE_FRONT, D3D12_FILL_MODE_SOLID)
		.SetDSVFormat(DXGI_FORMAT_D32_FLOAT)
		.Build(device->GetDevice());

	// 関数セット
	setViewFunction_ = func;
}

void ShadowRenderer::DrawCall(ID3D12GraphicsCommandList* list) {
	// 描画命令
	std::function<void(ID3D12GraphicsCommandList*)> draw = [this](ID3D12GraphicsCommandList* list) {
		// PSOを設定
		list->SetPipelineState(pso_.GetState());
		// ディスクリプタテーブルを登録
		list->SetGraphicsRootDescriptorTable(1, indexBuffer_.GetGPUView());
		// 全三角形を１つのDrawCallで描画
		list->DrawInstanced(3, indexBuffer_.GetCount() / 3, 0, 0);
	};

	// RootSignatureを設定
	list->SetGraphicsRootSignature(root_);
	// Viewを先にセット
	setViewFunction_();

	// ターゲット分ループする（平行光源）
	for (const Target<SM_Direction>& t : targetDir_) {
		// 描画先のDSVを設定する
		list->OMSetRenderTargets(0, nullptr, false, &t.shadow->dsvInfo.cpuView);

		// バリアを書き込み用に
		t.shadow->ChangeResourceBarrier(D3D12_RESOURCE_STATE_DEPTH_WRITE, list);
		// 画面全体をクリア
		t.shadow->Clear(list);

		// ビューポート
		D3D12_VIEWPORT viewport = {};
		// シャドウマップ用のテクスチャと同じサイズにする
		viewport.Width = static_cast<float>(t.shadow->width);
		viewport.Height = static_cast<float>(t.shadow->height);
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
		scissorRect.right = t.shadow->width;
		scissorRect.top = 0;
		scissorRect.bottom = t.shadow->height;
		// Scirssorを設定
		list->RSSetScissorRects(1, &scissorRect);

		// ViewProjectionをセット
		list->SetGraphicsRootConstantBufferView(0, t.view);

		// 描画
		draw(list);

		// バリアを戻す
		t.shadow->ChangeResourceBarrier(D3D12_RESOURCE_STATE_GENERIC_READ, list);
	}

	// ターゲット分ループする（ポイントライト）
	for (const Target<SM_Point>& t : targetPoint_) {
		// バリアを書き込み用に
		t.shadow->ChangeResourceBarrier(D3D12_RESOURCE_STATE_DEPTH_WRITE, list);
		// 画面全体をクリア
		t.shadow->Clear(list);

		// Viewをセット
		list->SetGraphicsRootConstantBufferView(1, t.view);

		// ビューポート
		D3D12_VIEWPORT viewport = {};
		// シャドウマップ用のテクスチャと同じサイズにする
		viewport.Width = static_cast<float>(t.shadow->width);
		viewport.Height = static_cast<float>(t.shadow->height);
		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;

		// シザー矩形
		D3D12_RECT scissorRect = {};
		// 基本的にビューポートと同じ矩形が構成されるようにする
		scissorRect.left = 0;
		scissorRect.right = t.shadow->width;
		scissorRect.top = 0;
		scissorRect.bottom = t.shadow->height;

		// ポイントライトなので6回レンダリング
		D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle[6] = {
			t.shadow->dsvInfo.cpuView,
			t.shadow->dsvInfos[0].cpuView,
			t.shadow->dsvInfos[1].cpuView,
			t.shadow->dsvInfos[2].cpuView,
			t.shadow->dsvInfos[3].cpuView,
			t.shadow->dsvInfos[4].cpuView
		};
		for (int i = 0; i < 6; i++) {
			// 描画先のDSVを設定する
			list->OMSetRenderTargets(0, nullptr, false, &dsvHandle[i]);

			// viewportを設定
			list->RSSetViewports(1, &viewport);
			// Scirssorを設定
			list->RSSetScissorRects(1, &scissorRect);
			// 描画
			draw(list);
		}

		// バリアを戻す
		t.shadow->ChangeResourceBarrier(D3D12_RESOURCE_STATE_GENERIC_READ, list);
	}
}


void ShadowRenderer::Reset() {
	targetDir_.clear();
	targetPoint_.clear();
	
	indexBuffer_.Reset();
}