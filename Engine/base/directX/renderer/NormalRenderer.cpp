#include "NormalRenderer.h"

#include "base/ImGuiManager.h"
#include "Config.h"

using namespace LWP::Base;

// サイズをここで指定
NormalRenderer::NormalRenderer() :
	normal_(lwpC::Rendering::kMaxIndex),
	wireframe_(lwpC::Rendering::kMaxIndex),
	billboard2D_(lwpC::Rendering::kMaxIndex),
	billboard3D_(lwpC::Rendering::kMaxIndex) {}

void NormalRenderer::Init(GPUDevice* device, SRV* srv, RootSignature* root, DXC* dxc, std::function<void()> func) {
	// StructuredBufferを初期化
	normal_.indexBuffer.Init(device, srv);
	wireframe_.indexBuffer.Init(device, srv);
	billboard2D_.indexBuffer.Init(device, srv);
	billboard3D_.indexBuffer.Init(device, srv);

	// PSOを生成
	normal_.pso.Init(*root, dxc)
		.SetVertexShader("Object3d.VS.hlsl")
		.SetPixelShader("Object3d.PS.hlsl")
		.Build(device->GetDevice());
	wireframe_.pso.Init(*root, dxc)
		.SetRasterizerState(D3D12_CULL_MODE_NONE, D3D12_FILL_MODE_WIREFRAME)
		.SetVertexShader("Object3d.VS.hlsl")
		.SetPixelShader("Object3d.PS.hlsl")
		.Build(device->GetDevice());
	billboard2D_.pso.Init(*root, dxc)
		.SetVertexShader("Billboard2D.VS.hlsl")
		.SetPixelShader("Billboard.PS.hlsl")
		.Build(device->GetDevice());
	billboard3D_.pso.Init(*root, dxc)
		.SetVertexShader("Billboard3D.VS.hlsl")
		.SetPixelShader("Billboard.PS.hlsl")
		.Build(device->GetDevice());

	// 関数セット
	setViewFunction_ = func;
}

void NormalRenderer::DrawCall(ID3D12GraphicsCommandList* list) {
	// 配列化
	RenderData* ary[4] = { &normal_, &wireframe_, &billboard2D_, &billboard3D_ };

	// Viewを先にセット
	setViewFunction_();

	// ターゲット分ループする
	for (std::vector<Target>::iterator it = target_.begin(); it != target_.end(); ++it) {
		// 描画先のRTVとDSVを設定する
		list->OMSetRenderTargets(1, &it->back->rtvInfo.cpuView, false, &it->depth->dsvInfo.cpuView);
		
		// リソースバリアをセット
		D3D12_RESOURCE_STATES beforeBarrier = it->back->GetBarrier();
		it->back->ChangeResourceBarrier(D3D12_RESOURCE_STATE_RENDER_TARGET, list);
		// 全画面クリア
		//it->back->Clear(list);
		// 指定した深度で画面全体をクリアする
		//it->depth->Clear(list);

		// ビューポート
		D3D12_VIEWPORT viewport = {};
		// クライアント領域のサイズと一緒にして画面全体に表示
		viewport.Width = static_cast<float>(it->back->width);
		viewport.Height = static_cast<float>(it->back->height);
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
		scissorRect.right = it->back->width;
		scissorRect.top = 0;
		scissorRect.bottom = it->back->height;
		// Scirssorを設定
		list->RSSetScissorRects(1, &scissorRect);

		// Viewをセット
		list->SetGraphicsRootConstantBufferView(1, it->view);

		// 4種類のデータをDraw
		for (RenderData* data : ary) {
			// PSOを設定
			list->SetPipelineState(data->pso.GetState());
			// ディスクリプタテーブルを登録
			list->SetGraphicsRootDescriptorTable(0, data->indexBuffer.GetGPUView());
			// 全三角形を１つのDrawCallで描画
			list->DrawInstanced(3, data->indexBuffer.GetCount() / 3, 0, 0);
		}

		// バリアを元に戻す
		it->back->ChangeResourceBarrier(beforeBarrier, list);
	}
}

void NormalRenderer::Reset() {
	target_.clear();

	normal_.indexBuffer.Reset();
	wireframe_.indexBuffer.Reset();
	billboard2D_.indexBuffer.Reset();
	billboard3D_.indexBuffer.Reset();
}

