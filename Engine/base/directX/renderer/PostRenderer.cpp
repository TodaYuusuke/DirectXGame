#include "PostRenderer.h"

#include "base/ImGuiManager.h"
#include "Config.h"

using namespace LWP::Base;

// サイズをここで指定
PostRenderer::PostRenderer() : sprite_(lwpC::Rendering::kMaxIndex) {}

void PostRenderer::Init(RootSignature* root, std::function<void()> func) {
	// StructuredBufferを初期化
	sprite_.indexBuffer.Init();

	// PSOを生成
	sprite_.pso.Init(*root)
		.SetVertexShader("Object3d.VS.hlsl")
		.SetPixelShader("Object3d.PS.hlsl")
		.Build();

	// 関数セット
	setViewFunction_ = func;
}

void PostRenderer::DrawCall(ID3D12GraphicsCommandList* list) {
	// ImGuiの読み込み終了
	ImGui::EndFrame();

	// Viewを先にセット
	setViewFunction_();

	// ターゲット分ループする
	for (std::vector<Target>::iterator it = target_.begin(); it != target_.end(); ++it) {
		// 描画先のRTVとDSVを設定する
		list->OMSetRenderTargets(1, &it->back->rtvInfo.cpuView, false, &it->depth->dsvInfo.cpuView);

		// リソースバリアをセット
		D3D12_RESOURCE_STATES beforeBarrier = it->back->GetBarrier();
		it->back->ChangeResourceBarrier(D3D12_RESOURCE_STATE_RENDER_TARGET, list);

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

		// SpriteはBackBufferにのみレンダリングする（フィードバックループ対策）
		list->SetPipelineState(sprite_.pso.GetState());	// PSOを設定
		list->SetGraphicsRootDescriptorTable(0, sprite_.indexBuffer.GetGPUView());	// ディスクリプタテーブルを登録
		list->DrawInstanced(3, sprite_.indexBuffer.GetCount() / 3, 0, 0);	// 全三角形を１つのDrawCallで描画
		// ImGuiをDraw
		ImGui::Render();
		ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), list);

		// バリアを元に戻す
		it->back->ChangeResourceBarrier(beforeBarrier, list);
	}
}

void PostRenderer::Reset() {
	target_.clear();

	sprite_.indexBuffer.Reset();
}

