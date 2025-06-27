#pragma once
#include "DrawSolid.h"

#include "object/ObjectManager.h"
#include "resources/ResourceManager.h"

using namespace LWP::Resource;

namespace LWP::Base {
	void DrawSolid::Init() {
		root_.AddCBVParameter(0, SV_Mesh)	// 0 カメラ
			.AddTableParameter(0, SV_Pixel, 1, lwpC::Rendering::kMaxTexture)	// 1 テクスチャ
			.AddSampler(0, SV_Pixel)		// x テクスチャ用サンプラー
			.AddCBVParameter(1, SV_All)		// 2 メタデータ
			.AddTableParameter(0, SV_All)	// 3 メッシュレット
			.AddTableParameter(1, SV_All)	// 4 頂点
			.AddTableParameter(2, SV_All)	// 5 ユニーク頂点
			.AddTableParameter(3, SV_All)	// 6 プリミティブインデックス
			.AddTableParameter(4, SV_Mesh)	// 7 ワールドトランスフォーム
			.AddTableParameter(5, SV_Pixel)	// 8 マテリアル
			.Build();
#pragma region Rigid描画
		psos_[ModelType::Rigid][0].Init(root_, PSO::Type::Mesh)
			.SetSystemAS("Rework_/graphics/Solid.AS.hlsl")
			.SetSystemMS("Rework_/graphics/Rigid.MS.hlsl")
			.SetSystemPS("Rework_/graphics/Solid.PS.hlsl")
			.Build();
		psos_[ModelType::Rigid][1].Copy(psos_[ModelType::Rigid][0])
			.SetRasterizerState(D3D12_CULL_MODE_NONE, D3D12_FILL_MODE_WIREFRAME)
			.Build();
#pragma endregion
	} 

	void DrawSolid::PushCommand(ID3D12GraphicsCommandList6* list) {
		// ルートシグネチャをセット
		list->SetGraphicsRootSignature(root_);
		
		// 共通で利用するレンダリング用のバッファーを設定する
		SetBuffers(list);
		// 各カメラに対しての描画命令を積み込む
		for (Object::Camera* camera : Object::Manager::GetInstance()->GetCameras()) {
			if (!camera->isActive) { continue; }	// カメラがアクティブでない場合はスキップ

			RenderResource * renderResource = camera->GetRenderResource();	// レンダリングターゲットを取得
			DepthStencil* depthStencil = camera->GetDepthStencil();			// 深度ステンシルを取得

			// バリアを設定
			renderResource->ChangeResourceBarrier(D3D12_RESOURCE_STATE_RENDER_TARGET, list);
			depthStencil->ChangeResourceBarrier(D3D12_RESOURCE_STATE_DEPTH_WRITE, list);	// デプスはずっとこのバリアなのでスルー

			list->OMSetRenderTargets(1, &renderResource->rtvInfo.cpuView, false, &depthStencil->dsvInfo.cpuView);	// レンダーターゲットを設定
			list->SetGraphicsRootConstantBufferView(0, camera->GetBufferView());	// カメラのバッファを登録
			D3D12_VIEWPORT viewport = renderResource->GetViewport();	// ビューポートを取得して設定
			list->RSSetViewports(1, &viewport);
			D3D12_RECT scissorRect = renderResource->GetScissorRect();	// シザー矩形を取得して設定
			list->RSSetScissorRects(1, &scissorRect);
			SetDispatchMesh(list);

			// バリアを元に戻す
			renderResource->RevertResourceBarrier(list);
			depthStencil->RevertResourceBarrier(list);	// 上記と同様
		}
	}


	void DrawSolid::SetBuffers(ID3D12GraphicsCommandList6* list) {
		SRV* srv = SRV::GetInstance();
		list->SetGraphicsRootDescriptorTable(1, srv->GetFirstTexView());	// テクスチャのバッファを登録
	}
	void DrawSolid::SetDispatchMesh(ID3D12GraphicsCommandList6* list) {
		// 全モデル分ループ
		auto models = Resource::Manager::GetInstance()->GetModels();
		
		// RigidModelをDispatch
		for (Models& m : models) {
			// ModelのメッシュレットのViewをセット
			ModelData& d = m.data;
			list->SetGraphicsRootDescriptorTable(3, d.buffers_.meshlet->GetGPUView());
			list->SetGraphicsRootDescriptorTable(4, d.buffers_.vertex->GetGPUView());
			list->SetGraphicsRootDescriptorTable(5, d.buffers_.uniqueVertexIndices->GetGPUView());
			list->SetGraphicsRootDescriptorTable(6, d.buffers_.primitiveIndices->GetGPUView());

			Models::FillMode<RigidModel, Models::RigidBuffer>& f = m.rigid;
			// Solidの描画処理
			if (!f.solid.ptrs.list.empty()) {
				// 各インスンタンスのバッファをセット
				list->SetGraphicsRootConstantBufferView(2, f.solid.buffer.common.GetGPUView());
				list->SetGraphicsRootDescriptorTable(7, f.solid.buffer.inst->GetGPUView());
				list->SetGraphicsRootDescriptorTable(8, f.solid.buffer.material->GetGPUView());
				list->SetPipelineState(psos_[ModelType::Rigid][0]);	// PSOセット
				list->DispatchMesh(d.GetMeshletCount(), 1, 1);	// メッシュレットのプリミティブ数分メッシュシェーダーを実行
			}
			// WireFrameの描画処理
			if (!f.wireFrame.ptrs.list.empty()) {
				// 追加のViewをセット
				list->SetGraphicsRootConstantBufferView(2, f.wireFrame.buffer.common.GetGPUView());
				list->SetGraphicsRootDescriptorTable(7, f.wireFrame.buffer.inst->GetGPUView());
				list->SetGraphicsRootDescriptorTable(8, f.wireFrame.buffer.material->GetGPUView());
				list->SetPipelineState(psos_[ModelType::Rigid][1]);	// PSOセット
				list->DispatchMesh(d.GetMeshletCount(), 1, 1);	// メッシュレットのプリミティブ数分メッシュシェーダーを実行
			}
		}
	}
}