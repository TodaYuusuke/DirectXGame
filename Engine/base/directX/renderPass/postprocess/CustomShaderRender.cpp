#include "CustomShaderRender.h"

#include "object/ObjectManager.h"
#include "base/directX/resource/rendering/BackBuffer.h"

namespace LWP::Base {
	void CustomShaderRender::Init() {/* 何もない! */ }

	void CustomShaderRender::PushCommand(BackBuffer* target, ID3D12GraphicsCommandList6* list) {
		// 各カメラに対しての描画命令を積み込む
		for (Object::Camera* camera : Object::Manager::GetInstance()->GetCameras()) {
			if (!camera->isActive) { continue; }	// カメラがアクティブでない場合はスキップ
			if (!camera->pp.use) { continue; }		// ポストプロセスを使用しない場合はスキップ

			// 事前に行わなければならない処理を行う
			camera->pp.PreCommands(list, camera->GetTextureResource());

			// ターゲットの方に書き込む
			list->OMSetRenderTargets(1, &target->rtvInfo.cpuView, false, nullptr);

			// データを登録
			camera->pp.SetCommands(list);
			// 既定値のViewをバインド
			list->SetGraphicsRootDescriptorTable(1, camera->GetTextureResource()->srvInfo.gpuView);
			list->SetGraphicsRootDescriptorTable(2, camera->GetGBuffer()->depthStencil.srvInfo.gpuView);

			// リソースバリアをセット
			D3D12_RESOURCE_STATES beforeBarrier = target->GetBarrier();
			camera->GetTextureResource()->ChangeResourceBarrier(D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, list);	// 加工する画像
			target->ChangeResourceBarrier(D3D12_RESOURCE_STATE_RENDER_TARGET, list);	// 書き込み対象
			camera->GetGBuffer()->depthStencil.ChangeResourceBarrier(D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, list);	// 深度マップ

			// ビューポート
			D3D12_VIEWPORT viewport = target->GetViewport();
			list->RSSetViewports(1, &viewport);

			// シザー矩形
			D3D12_RECT scissorRect = target->GetScissorRect();
			list->RSSetScissorRects(1, &scissorRect);

			// 描画!
			list->DrawInstanced(3, 1, 0, 0);

			// バリアを元に戻す
			camera->GetTextureResource()->ChangeResourceBarrier(D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, list);	// 加工する画像
			target->ChangeResourceBarrier(beforeBarrier, list);	// 書き込み対象
			camera->GetGBuffer()->depthStencil.ChangeResourceBarrier(D3D12_RESOURCE_STATE_DEPTH_WRITE, list);	// 深度マップ
		}
	}
}