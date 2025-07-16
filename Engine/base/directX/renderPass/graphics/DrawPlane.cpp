#pragma once
#include "DrawPlane.h"

#include "object/ObjectManager.h"
#include "primitive/PrimitiveManager.h"

using namespace LWP::Primitive;

namespace LWP::Base {
	void DrawPlane::Init() {
		// ルートシグネチャを設定
		root_.AddTableParameter(0, SV_Vertex)	// 0 頂点
			.AddTableParameter(1, SV_Vertex)	// 1 ワールドトランスフォーム
			.AddCBVParameter(0, SV_Vertex)		// 2 カメラ
			.AddTableParameter(2, SV_Vertex)		// 3 Zソートインデックス
			.AddTableParameter(0, SV_Pixel)		// 4 マテリアル
			.AddTableParameter(0, SV_Pixel, 1, lwpC::Rendering::kMaxTexture)	// 5 テクスチャ
			.AddSampler(0, SV_Pixel)			// x テクスチャ用サンプラー
			.Build();

		// スプライトの描画
		psos_.sprite.Init(root_, PSO::Type::Vertex)
			.SetSystemVS("Rework_/graphics/plane/Sprite.VS.hlsl")
			.SetSystemPS("Rework_/graphics/plane/Plane.PS.hlsl")
			.Build();
		// ビルボード3Dの描画
		psos_.billboard3D.Copy(psos_.sprite)
			.SetSystemVS("Rework_/graphics/plane/Billboard2D.VS.hlsl")
			.Build();
	}

	void DrawPlane::PushCommand(ID3D12GraphicsCommandList6* list) {
		// ルートシグネチャをセット
		list->SetGraphicsRootSignature(root_);
		// 共通のバッファを登録する
		SetBuffers(list);

		// 各カメラに対しての描画命令を積み込む
		for (Object::Camera* camera : Object::Manager::GetInstance()->GetCameras()) {
			if (!camera->isActive) { continue; }	// カメラがアクティブでない場合はスキップ
			// リソースを取得
			RenderResource* renderResource = camera->GetTextureResource();
			DepthStencil* depthStencil = &camera->GetGBuffer()->depthStencil;

			// レンダーターゲットに指定
			list->OMSetRenderTargets(1, &renderResource->rtvInfo.cpuView, FALSE, &depthStencil->dsvInfo.cpuView);

			// 各リソースのバリアを設定
			renderResource->ChangeResourceBarrier(D3D12_RESOURCE_STATE_RENDER_TARGET, list);
			depthStencil->ChangeResourceBarrier(D3D12_RESOURCE_STATE_DEPTH_WRITE, list);

			// ビューポートを取得して設定
			D3D12_VIEWPORT viewport = renderResource->GetViewport();
			list->RSSetViewports(1, &viewport);
			// シザー矩形を取得して設定
			D3D12_RECT scissorRect = renderResource->GetScissorRect();
			list->RSSetScissorRects(1, &scissorRect);

			// カメラのバッファを登録
			list->SetGraphicsRootConstantBufferView(2, camera->GetBufferView());

			// 描画！
			SetDrawCall(list);
		}
	}


	void DrawPlane::SetBuffers(ID3D12GraphicsCommandList6* list) {
		SRV* srv = SRV::GetInstance();
		list->SetGraphicsRootDescriptorTable(5, srv->GetFirstTexView());	// テクスチャのバッファを登録
	}
	void DrawPlane::SetDrawCall(ID3D12GraphicsCommandList6* list) {
		// Spriteの描画
		DrawSprite(list);
		DrawBillboard2D(list);
	}

	void DrawPlane::DrawSprite(ID3D12GraphicsCommandList6* list) {
		list->SetPipelineState(psos_.sprite);	// PSOを設定
		DrawCommon(Primitive::Manager::GetInstance()->GetSpriteBuffer(), list);
	}
	void DrawPlane::DrawBillboard2D(ID3D12GraphicsCommandList6* list) {
		list->SetPipelineState(psos_.billboard3D);	// PSOを設定
		DrawCommon(Primitive::Manager::GetInstance()->GetBillboard2DBuffer(), list);
	}

	void DrawPlane::DrawCommon(Primitive::PlaneBuffers* buffers, ID3D12GraphicsCommandList6* list) {
		// インスタンスがないなら描画を行わない
		if (buffers->count <= 0) { return; }

		// Viewを設定
		list->SetGraphicsRootDescriptorTable(0, buffers->vertices.GetGPUView());		// 頂点
		list->SetGraphicsRootDescriptorTable(1, buffers->wtf.GetGPUView());			// ワールドトランスフォーム
		list->SetGraphicsRootDescriptorTable(4, buffers->materials.GetGPUView());	// マテリアル
		list->SetGraphicsRootDescriptorTable(3, Primitive::Manager::GetInstance()->GetZSortBuffer()->GetGPUView());	// マテリアル
		list->DrawInstanced(6, buffers->count, 0, 0);
	}
}
