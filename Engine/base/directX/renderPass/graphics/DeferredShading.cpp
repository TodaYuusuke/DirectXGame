#pragma once
#include "DeferredShading.h"

#include "object/ObjectManager.h"
#include "resources/ResourceManager.h"
#include "utility/MyUtility.h"

using namespace LWP::Resource;

namespace LWP::Base {
	void DeferredShading::Init() {
		// ルートシグネチャの設定
		root_.AddTableParameter(0, SV_Pixel)	// 0 アルベド
			.AddTableParameter(1, SV_Pixel)		// 1 法線
			.AddTableParameter(2, SV_Pixel)		// 2 マテリアル
			.AddTableParameter(3, SV_Pixel)		// 3 ワールド座標
			.AddSampler(0, SV_Pixel)			// x 各リソース用のサンプラー
			.AddCBVParameter(0, SV_Pixel)		// 4 ライト系のメタデータ
			.AddCBVParameter(1, SV_Pixel)		// 5 平行光源のデータ
			.Build();
		// パススルー描画の後、遅延ライテイング描画
		pso_.Init(root_, PSO::Type::Vertex)
			.SetDepthState(false)
			.SetSystemVS("Rework_/utility/PassThrough.VS.hlsl")
			.SetSystemPS("Rework_/graphics/SolidLighting.PS.hlsl")
			.Build();
	} 

	void DeferredShading::PushCommand(ID3D12GraphicsCommandList6* list) {
		list->SetGraphicsRootSignature(root_);	// ルートシグネチャをセット
		list->SetPipelineState(pso_);	// PSOセット

		// 共通で利用するレンダリング用のバッファーを設定する
		SetBuffers(list);
		// 各カメラに対しての描画命令を積み込む
		for (Object::Camera* camera : Object::Manager::GetInstance()->GetCameras()) {
			if (!camera->isActive) { continue; }	// カメラがアクティブでない場合はスキップ

			GBuffer* g = camera->GetGBuffer();
			g->SetShaderResource(list);	// GBufferにバリアの設定を任せる

			// 描画対象をセット
			list->OMSetRenderTargets(1, &camera->GetTextureResource()->rtvInfo.cpuView, false, nullptr);

			// カメラのもつ各リソースのバインドを行う
			list->SetGraphicsRootDescriptorTable(0, g->albedo.srvInfo.gpuView);
			list->SetGraphicsRootDescriptorTable(1, g->normal.srvInfo.gpuView);
			list->SetGraphicsRootDescriptorTable(2, g->pbr.srvInfo.gpuView);
			list->SetGraphicsRootDescriptorTable(3, g->worldPosition.srvInfo.gpuView);

			// 描画命令はこれだけ
			list->DrawInstanced(3, 1, 0, 0);
		}
	}


	void DeferredShading::SetBuffers(ID3D12GraphicsCommandList6* list) {
		//SRV* srv = SRV::GetInstance();
		Object::Manager* objManager = Object::Manager::GetInstance();

		// 光源達のメタデータをセット
		list->SetGraphicsRootConstantBufferView(4, objManager->GetLightMetadataBuffer().GetGPUView());
		// 平行光源のデータをセット
		list->SetGraphicsRootConstantBufferView(5, objManager->GetDirectionLight()->GetLightBuffer().GetGPUView());
	}
}