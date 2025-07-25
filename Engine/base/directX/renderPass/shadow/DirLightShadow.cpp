#pragma once
#include "DirLightShadow.h"

#include "object/ObjectManager.h"
#include "resources/ResourceManager.h"

using namespace LWP::Resource;

#define RS_SLOT_LightView			0   // ライトのビュー投影行列 (b0)
#define RS_SLOT_MetaData			1   // モデルのメタデータ (b1)
#define RS_SLOT_Meshlet				2   // メッシュレット構造体 (t0)
#define RS_SLOT_Vertex				3   // 頂点情報 (t1)
#define RS_SLOT_UniqueVertexIndices 4   // ユニーク頂点インデックス (t2)
#define RS_SLOT_PrimitiveIndices	5   // プリミティブインデックス (t3)
#define RS_SLOT_WorldTF				6   // ワールドトランスフォーム (t4)
#define RS_SLOT_Well				7   // Skinning用のウェル (t5)

namespace LWP::Base {
	void DirLightShadow::Init() {
		// ルートシグネチャを設定
		root_.AddCBVParameter(0, SV_Mesh)	// 0 ライトのビュー投影行列
			.AddCBVParameter(1, SV_All)		// 1 メタデータ
			.AddTableParameter(0, SV_Mesh)	// 2 メッシュレット
			.AddTableParameter(1, SV_Mesh)	// 3 頂点
			.AddTableParameter(2, SV_Mesh)	// 4 ユニーク頂点
			.AddTableParameter(3, SV_Mesh)	// 5 プリミティブインデックス
			.AddTableParameter(4, SV_Mesh)	// 6 ワールドトランスフォーム
			.AddTableParameter(5, SV_Mesh)	// 7 Well（Skin用）
			.Build();

		// psoの設定
		psos_[ModelType::Rigid].Init(root_, PSO::Type::Mesh)
			.SetSystemAS("Rework_/shadow/Shadow.AS.hlsl")
			.SetSystemMS("Rework_/shadow/RigidShadow.MS.hlsl")
			.Build();
		psos_[ModelType::Skin].Init(root_, PSO::Type::Mesh)
			.SetSystemAS("Rework_/shadow/Shadow.AS.hlsl")
			.SetSystemMS("Rework_/shadow/SkinShadow.MS.hlsl")
			.Build();
	}

	void DirLightShadow::PushCommand(ID3D12GraphicsCommandList6* list) {
		// ルートシグネチャをセット
		list->SetGraphicsRootSignature(root_);

		// 平行光源のシャドウマッピングを行う
		Object::DirectionLight* dir = Object::Manager::GetInstance()->GetDirLight();
		if (!dir->isActive) { return; }
		SM_Direction* shadowMap = dir->GetShadowMap();
		
		list->OMSetRenderTargets(0, nullptr, false, &shadowMap->dsvInfo.cpuView);	// レンダーターゲットに指定
		shadowMap->ChangeResourceBarrier(D3D12_RESOURCE_STATE_DEPTH_WRITE, list);	// バリアを書き込み用に
		shadowMap->Clear(list);									// 画面クリア
		D3D12_VIEWPORT viewport = shadowMap->GetViewport();		// ビューポートを取得して設定
		list->RSSetViewports(1, &viewport);
		D3D12_RECT scissorRect = shadowMap->GetScissorRect();	// シザー矩形を取得して設定
		list->RSSetScissorRects(1, &scissorRect);

		list->SetGraphicsRootConstantBufferView(RS_SLOT_LightView, dir->GetMatrixBufferView());	// 光源のビュープロジェクションを登録

		// 描画！
		SetDispatchMesh(list);
	}

	void DirLightShadow::SetDispatchMesh(ID3D12GraphicsCommandList6* list) {
		// 全モデル分ループ
		auto models = Resource::Manager::GetInstance()->GetModels();
		for (Models& m : models) {
			// ModelのメッシュレットのViewをセット
			ModelData& d = m.data;
			list->SetGraphicsRootDescriptorTable(RS_SLOT_Meshlet, d.buffers_.meshlet->GetGPUView());
			list->SetGraphicsRootDescriptorTable(RS_SLOT_Vertex, d.buffers_.vertex->GetGPUView());
			list->SetGraphicsRootDescriptorTable(RS_SLOT_UniqueVertexIndices, d.buffers_.uniqueVertexIndices->GetGPUView());
			list->SetGraphicsRootDescriptorTable(RS_SLOT_PrimitiveIndices, d.buffers_.primitiveIndices->GetGPUView());

			// ** ---------- RigidModelをDispatch ---------- ** //
			Models::FillMode<RigidModel, Models::RigidBuffer>& r = m.rigid;
			// Solidの描画処理
			if (!r.solid.ptrs.list.empty()) {
				// 各インスンタンスのバッファをセット
				list->SetGraphicsRootConstantBufferView(RS_SLOT_MetaData, r.solid.buffer.common.GetGPUView());
				list->SetGraphicsRootDescriptorTable(RS_SLOT_WorldTF, r.solid.buffer.inst->GetGPUView());
				list->SetPipelineState(psos_[ModelType::Rigid]);	// PSOセット
				list->DispatchMesh(d.GetMeshletCount(), 1, 1);	// メッシュレットのプリミティブ数分メッシュシェーダーを実行
			}
			// WireFrameは描画しない

			// ** ---------- SkinModelをDispatch ---------- ** 
			Models::FillMode<SkinningModel, Models::SkinBuffer>& s = m.skin;
			// Solidの描画処理
			if (!s.solid.ptrs.list.empty()) {
				// 各インスンタンスのバッファをセット
				list->SetGraphicsRootConstantBufferView(RS_SLOT_MetaData, s.solid.buffer.common.GetGPUView());
				list->SetGraphicsRootDescriptorTable(RS_SLOT_WorldTF, s.solid.buffer.inst->GetGPUView());
				list->SetGraphicsRootDescriptorTable(RS_SLOT_Well, s.solid.buffer.well->GetGPUView());
				list->SetPipelineState(psos_[ModelType::Skin]);	// PSOセット
				list->DispatchMesh(d.GetMeshletCount(), 1, 1);	// メッシュレットのプリミティブ数分メッシュシェーダーを実行
			}
			// WireFrameは描画しない
		}
	}
}