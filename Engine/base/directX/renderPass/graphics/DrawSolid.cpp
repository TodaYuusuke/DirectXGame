#pragma once
#include "DrawSolid.h"

#include "object/ObjectManager.h"
#include "resources/ResourceManager.h"

using namespace LWP::Resource;

namespace LWP::Base {
	void DrawSolid::Init() {
		// ルートシグネチャを設定
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
			.AddTableParameter(5, SV_Mesh)	// 9 Well（Skin用）
			.Build();

#pragma region Rigid描画
		raster_[ModelType::Rigid].solid.Init(root_, PSO::Type::Mesh)
			.SetRTVFormats({
				DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
				DXGI_FORMAT_R10G10B10A2_UNORM,
				DXGI_FORMAT_R8G8B8A8_UNORM,
				DXGI_FORMAT_R16G16B16A16_FLOAT,
			})
			.SetSystemAS("Rework_/graphics/Solid.AS.hlsl")
			.SetSystemMS("Rework_/graphics/Rigid.MS.hlsl")
			.SetSystemPS("Rework_/graphics/Solid.PS.hlsl")
			.Build();
		raster_[ModelType::Rigid].wireframe.Copy(raster_[ModelType::Rigid].solid)
			.SetRasterizerState(D3D12_CULL_MODE_NONE, D3D12_FILL_MODE_WIREFRAME)
			.Build();
#pragma endregion
#pragma region Skin描画
		raster_[ModelType::Skin].solid.Init(root_, PSO::Type::Mesh)
			.SetRTVFormats({
				DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
				DXGI_FORMAT_R10G10B10A2_UNORM,
				DXGI_FORMAT_R8G8B8A8_UNORM,
				DXGI_FORMAT_R16G16B16A16_FLOAT,
			})
			.SetSystemAS("Rework_/graphics/Solid.AS.hlsl")
			.SetSystemMS("Rework_/graphics/Skin.MS.hlsl")
			.SetSystemPS("Rework_/graphics/Solid.PS.hlsl")
			.Build();
		raster_[ModelType::Skin].wireframe.Copy(raster_[ModelType::Skin].solid)
			.SetRasterizerState(D3D12_CULL_MODE_NONE, D3D12_FILL_MODE_WIREFRAME)
			.Build();
#pragma endregion
	} 

	void DrawSolid::PushCommand(ID3D12GraphicsCommandList6* list) {
		// ルートシグネチャをセット
		list->SetGraphicsRootSignature(root_);
		// 共通のバッファを登録する
		SetBuffers(list);

		// 各カメラに対しての描画命令を積み込む
		for (Object::Camera* camera : Object::Manager::GetInstance()->GetCameras()) {
			if (!camera->isActive) { continue; }	// カメラがアクティブでない場合はスキップ

			GBuffer* g = camera->GetGBuffer();
			g->SetRenderTarget(list);	// GBufferにレンダーターゲットセット、バリア、ビューポートとシザー矩形を任せる
			list->SetGraphicsRootConstantBufferView(0, camera->GetBufferView());	// カメラのバッファを登録

			// 描画！
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
		for (Models& m : models) {
			// ModelのメッシュレットのViewをセット
			ModelData& d = m.data;
			list->SetGraphicsRootDescriptorTable(3, d.buffers_.meshlet->GetGPUView());
			list->SetGraphicsRootDescriptorTable(4, d.buffers_.vertex->GetGPUView());
			list->SetGraphicsRootDescriptorTable(5, d.buffers_.uniqueVertexIndices->GetGPUView());
			list->SetGraphicsRootDescriptorTable(6, d.buffers_.primitiveIndices->GetGPUView());

			// ** ---------- RigidModelをDispatch ---------- ** //
			Models::FillMode<RigidModel, Models::RigidBuffer>& r = m.rigid;
			// Solidの描画処理
			if (!r.solid.ptrs.list.empty()) {
				// 各インスンタンスのバッファをセット
				list->SetGraphicsRootConstantBufferView(2, r.solid.buffer.common.GetGPUView());
				list->SetGraphicsRootDescriptorTable(7, r.solid.buffer.inst->GetGPUView());
				list->SetGraphicsRootDescriptorTable(8, r.solid.buffer.material->GetGPUView());
				list->SetPipelineState(raster_[ModelType::Rigid].solid);	// PSOセット
				list->DispatchMesh(d.GetMeshletCount(), 1, 1);	// メッシュレットのプリミティブ数分メッシュシェーダーを実行
			}
			// WireFrameの描画処理
			if (!r.wireFrame.ptrs.list.empty()) {
				// 追加のViewをセット
				list->SetGraphicsRootConstantBufferView(2, r.wireFrame.buffer.common.GetGPUView());
				list->SetGraphicsRootDescriptorTable(7, r.wireFrame.buffer.inst->GetGPUView());
				list->SetGraphicsRootDescriptorTable(8, r.wireFrame.buffer.material->GetGPUView());
				list->SetPipelineState(raster_[ModelType::Rigid].wireframe);	// PSOセット
				list->DispatchMesh(d.GetMeshletCount(), 1, 1);	// メッシュレットのプリミティブ数分メッシュシェーダーを実行
			}

			// ** ---------- SkinModelをDispatch ---------- ** 
			Models::FillMode<SkinningModel, Models::SkinBuffer>& s = m.skin;
			// Solidの描画処理
			if (!s.solid.ptrs.list.empty()) {
				// 各インスンタンスのバッファをセット
				list->SetGraphicsRootConstantBufferView(2, s.solid.buffer.common.GetGPUView());
				list->SetGraphicsRootDescriptorTable(7, s.solid.buffer.inst->GetGPUView());
				list->SetGraphicsRootDescriptorTable(8, s.solid.buffer.material->GetGPUView());
				list->SetGraphicsRootDescriptorTable(9, s.solid.buffer.well->GetGPUView());
				list->SetPipelineState(raster_[ModelType::Skin].solid);	// PSOセット
				list->DispatchMesh(d.GetMeshletCount(), 1, 1);	// メッシュレットのプリミティブ数分メッシュシェーダーを実行
			}
			// WireFrameの描画処理
			if (!s.wireFrame.ptrs.list.empty()) {
				// 追加のViewをセット
				list->SetGraphicsRootConstantBufferView(2, s.wireFrame.buffer.common.GetGPUView());
				list->SetGraphicsRootDescriptorTable(7, s.wireFrame.buffer.inst->GetGPUView());
				list->SetGraphicsRootDescriptorTable(8, s.wireFrame.buffer.material->GetGPUView());
				list->SetGraphicsRootDescriptorTable(9, s.wireFrame.buffer.well->GetGPUView());
				list->SetPipelineState(raster_[ModelType::Skin].wireframe);	// PSOセット
				list->DispatchMesh(d.GetMeshletCount(), 1, 1);	// メッシュレットのプリミティブ数分メッシュシェーダーを実行
			}
		}
	}
}