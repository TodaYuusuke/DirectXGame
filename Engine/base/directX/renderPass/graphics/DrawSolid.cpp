#pragma once
#include "DrawSolid.h"

#include "object/core/Camera.h"
#include "resources/ResourceManager.h"

using namespace LWP::Resource;

namespace LWP::Base {
	void DrawSolid::Init() {
		root_.AddCBVParameter(0, SV_Mesh)	// カメラ
			.AddTableParameter(0, SV_Pixel, 1, lwpC::Rendering::kMaxTexture)	// テクスチャ
			.AddSampler(0, SV_Pixel)		// テクスチャ用サンプラー
			.AddCBVParameter(1, SV_All)	// メタデータ
			.AddTableParameter(0, SV_All)	// メッシュレット
			.AddTableParameter(1, SV_All)	// 頂点
			.AddTableParameter(2, SV_All)	// ユニーク頂点
			.AddTableParameter(3, SV_All)	// プリミティブインデックス
			.AddTableParameter(4, SV_Mesh)	// ワールドトランスフォーム
			.AddTableParameter(5, SV_Pixel)	// マテリアル
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

	void DrawSolid::PushCommand(ID3D12GraphicsCommandList6* list, LWP::Object::Camera** cameras, int size) {
		// ルートシグネチャをセット
		list->SetGraphicsRootSignature(root_);
		
		// 共通で利用するレンダリング用のバッファーを設定する
		SetBuffers(list);
		// 各カメラに対しての描画命令を積み込む
		for (int i = 0; i < size; ++i) {
			RenderResource * renderResource = cameras[i]->GetRenderResource();	// レンダリングターゲットを取得
			DepthStencil* depthStencil = cameras[i]->GetDepthStencil();	// 深度ステンシルを取得

			list->OMSetRenderTargets(1, &renderResource->rtvInfo.cpuView, false, &depthStencil->dsvInfo.cpuView);	// レンダーターゲットを設定
			list->SetGraphicsRootConstantBufferView(0, cameras[i]->GetBufferView());	// カメラのバッファを登録
			list->RSSetViewports(1, &renderResource->GetViewport());	// ビューポートを設定
			list->RSSetScissorRects(1, &renderResource->GetScissorRect());	// シザー矩形を設定
			SetDispatchMesh(list);
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
			list->SetGraphicsRootDescriptorTable(4, d.buffers_.meshlet->GetGPUView());
			list->SetGraphicsRootDescriptorTable(5, d.buffers_.vertex->GetGPUView());
			list->SetGraphicsRootDescriptorTable(6, d.buffers_.uniqueVertexIndices->GetGPUView());
			list->SetGraphicsRootDescriptorTable(7, d.buffers_.primitiveIndices->GetGPUView());

			Models::FillMode<RigidModel, Models::RigidBuffer>& f = m.rigid;
			// Solidの描画処理
			if (!f.solid.ptrs.list.empty()) {
				// 各インスンタンスのバッファをセット
				list->SetGraphicsRootDescriptorTable(8, f.solid.buffer.inst->GetGPUView());
				list->SetGraphicsRootDescriptorTable(9, f.solid.buffer.material->GetGPUView());
				list->SetPipelineState(psos_[ModelType::Rigid][0]);	// PSOセット
				list->DispatchMesh(d.GetMeshletCount(), 1, 1);	// メッシュレットのプリミティブ数分メッシュシェーダーを実行
			}
			// WireFrameの描画処理
			if (!f.wireFrame.ptrs.list.empty()) {
				// 追加のViewをセット
				list->SetGraphicsRootDescriptorTable(8, f.wireFrame.buffer.inst->GetGPUView());
				list->SetGraphicsRootDescriptorTable(9, f.wireFrame.buffer.material->GetGPUView());
				list->SetPipelineState(psos_[ModelType::Rigid][1]);	// PSOセット
				list->DispatchMesh(d.GetMeshletCount(), 1, 1);	// メッシュレットのプリミティブ数分メッシュシェーダーを実行
			}
		}
	}
}