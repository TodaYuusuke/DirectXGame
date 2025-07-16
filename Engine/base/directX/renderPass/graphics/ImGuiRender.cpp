#pragma once
#include "ImGuiRender.h"

#include "base/ImGuiManager.h"
#include "../../resource/rendering/BackBuffer.h"

using namespace LWP::Resource;

namespace LWP::Base {
	void ImGuiRender::Init() { /* 何もなし */ } 

	void ImGuiRender::PushCommand(BackBuffer* backBuffer, ID3D12GraphicsCommandList6* list) {
		// バリアを設定
		backBuffer->ChangeResourceBarrier(D3D12_RESOURCE_STATE_RENDER_TARGET, list);

		list->OMSetRenderTargets(1, &backBuffer->rtvInfo.cpuView, false, nullptr);	// レンダーターゲットを設定
		D3D12_VIEWPORT viewport = backBuffer->GetViewport();	// ビューポートを取得して設定
		list->RSSetViewports(1, &viewport);
		D3D12_RECT scissorRect = backBuffer->GetScissorRect();	// シザー矩形を取得して設定
		list->RSSetScissorRects(1, &scissorRect);

		// ImGuiをDraw
		ImGui::Render();
		ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), list);

		// バリアを元に戻す
		backBuffer->RevertResourceBarrier(list);
	}
}