#include "PPRenderer.h"

#include "base/ImGuiManager.h"
#include "Config.h"

using namespace LWP::Base;

void PPRenderer::Init() {/* 何もない！ */}

void PPRenderer::DrawCall(ID3D12GraphicsCommandList* list) {
	// ターゲット分ループする
	for (std::vector<Target>::iterator it = target_.begin(); it != target_.end(); ++it) {
		// 事前に行わなければならない処理を行う
		it->pp->PreCommands(list, it->render);
		
		// textureの方に書き込む
		list->OMSetRenderTargets(1, &it->texture->rtvInfo.cpuView, false, nullptr);

		// データを登録
		it->pp->SetCommands(list);
		// 既定値のViewをバインド
		//list->SetGraphicsRootConstantBufferView(0, );
		list->SetGraphicsRootDescriptorTable(1, it->render->srvInfo.gpuView);
		list->SetGraphicsRootDescriptorTable(2, it->depth->srvInfo.gpuView);

		// リソースバリアをセット
		D3D12_RESOURCE_STATES beforeBarrier = it->texture->GetBarrier();
		it->render->ChangeResourceBarrier(D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, list);	// 加工する画像
		it->texture->ChangeResourceBarrier(D3D12_RESOURCE_STATE_RENDER_TARGET, list);	// 書き込み対象
		it->depth->ChangeResourceBarrier(D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, list);	// 深度マップ
		// 全画面クリア
		it->texture->Clear(list);

		// ビューポート
		D3D12_VIEWPORT viewport = {};
		// クライアント領域のサイズと一緒にして画面全体に表示
		viewport.Width = static_cast<float>(it->texture->width);
		viewport.Height = static_cast<float>(it->texture->height);
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
		scissorRect.right = it->texture->width;
		scissorRect.top = 0;
		scissorRect.bottom = it->texture->height;
		// Scirssorを設定
		list->RSSetScissorRects(1, &scissorRect);

		// 描画!
		list->DrawInstanced(3, 1, 0, 0);

		// バリアを元に戻す
		it->render->ChangeResourceBarrier(D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, list);	// 加工する画像
		it->texture->ChangeResourceBarrier(beforeBarrier, list);	// 書き込み対象
		it->depth->ChangeResourceBarrier(D3D12_RESOURCE_STATE_DEPTH_WRITE, list);	// 深度マップ
	}
}

void PPRenderer::Reset() {
	target_.clear();
}

