#include "GBuffer.h"

namespace LWP::Base {
	void GBuffer::Init() {
		// 全てのリソースを初期化
		albedo.Init();
		normal.Init();
		pbr.Init();
		worldPosition.Init();
		depthStencil.Init();
	}
	void GBuffer::Clear(ID3D12GraphicsCommandList* list) {
		// 各リソースのクリアを実行
		albedo.Clear(list);
		normal.Clear(list);
		pbr.Clear(list);
		worldPosition.Clear(list);
		depthStencil.Clear(list);
	}

	void GBuffer::SetRenderTarget(ID3D12GraphicsCommandList* list) {
		// 各リソースをまとめる
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles[] = {
			albedo.rtvInfo.cpuView,
			normal.rtvInfo.cpuView,
			pbr.rtvInfo.cpuView,
			worldPosition.rtvInfo.cpuView
		};
		// レンダーターゲットに指定
		list->OMSetRenderTargets(_countof(rtvHandles), rtvHandles, FALSE, &depthStencil.dsvInfo.cpuView);

		// 各リソースのバリアを設定
		albedo.ChangeResourceBarrier(D3D12_RESOURCE_STATE_RENDER_TARGET, list);
		normal.ChangeResourceBarrier(D3D12_RESOURCE_STATE_RENDER_TARGET, list);
		pbr.ChangeResourceBarrier(D3D12_RESOURCE_STATE_RENDER_TARGET, list);
		worldPosition.ChangeResourceBarrier(D3D12_RESOURCE_STATE_RENDER_TARGET, list);

		D3D12_VIEWPORT viewport = albedo.GetViewport();	// ビューポートを取得して設定
		list->RSSetViewports(1, &viewport);
		D3D12_RECT scissorRect = albedo.GetScissorRect();	// シザー矩形を取得して設定
		list->RSSetScissorRects(1, &scissorRect);
	}
	void GBuffer::SetShaderResource(ID3D12GraphicsCommandList* list) {
		// 各リソースのバリアを設定
		albedo.ChangeResourceBarrier(D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, list);
		normal.ChangeResourceBarrier(D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, list);
		pbr.ChangeResourceBarrier(D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, list);
		worldPosition.ChangeResourceBarrier(D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, list);

		D3D12_VIEWPORT viewport = albedo.GetViewport();	// ビューポートを取得して設定
		list->RSSetViewports(1, &viewport);
		D3D12_RECT scissorRect = albedo.GetScissorRect();	// シザー矩形を取得して設定
		list->RSSetScissorRects(1, &scissorRect);
	}

	void GBuffer::RevertResourceBarrier(ID3D12GraphicsCommandList* list) {
		// 各リソースのバリアを1つ前の状態に戻す
		albedo.RevertResourceBarrier(list);
		normal.RevertResourceBarrier(list);
	}

} // namespace LWP::Base