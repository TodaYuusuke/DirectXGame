#include "EnvironmentMapRenderer.h"

#include "component/Resource.h"

#include "Config.h"


using namespace LWP::Base;
using namespace LWP::Resource;

// サイズをここで指定
EnvironmentMapRenderer::EnvironmentMapRenderer() {}

void EnvironmentMapRenderer::Init(GPUDevice* device, SRV* srv, DXC* dxc, std::function<void()> func) {
	srv_ = srv;
	setViewFunction_ = func;

	// RootSignatureを生成
	rigid_.root.AddTableParameter(0, SV_All)	// メッシュレット
		.AddTableParameter(1, SV_All)	// 頂点
		.AddTableParameter(2, SV_All)	// ユニークポインタ
		.AddTableParameter(3, SV_All)	// プリミティブインデックス
		.AddCBVParameter(0, SV_All)	// モデルの共通データ
		.AddTableParameter(4, SV_All)	//	インデックスデータ
		.AddCBVParameter(1, SV_All)	// 全体共通のデータ
		.AddCBVParameter(2, SV_All)	// カメラのView
		.AddTableParameter(5, SV_Pixel)	// マテリアル
		.AddTableParameter(6, SV_Pixel)	// 平行光源
		.AddTableParameter(7, SV_Pixel)	// 点光源
		.AddTableParameter(8, SV_Pixel, 0, lwpC::Rendering::kMaxTexture)	// テクスチャ
		.AddTableParameter(508, SV_Pixel, 0, lwpC::Shadow::Direction::kMaxCount)	// 平行光源のシャドウマップ
		.AddTableParameter(509, SV_Pixel, 0, lwpC::Shadow::Point::kMaxCount)	// 点光源のシャドウマップ
		.AddSampler(0, SV_Pixel)		// テクスチャ用サンプラー
		.AddSampler(1, SV_Pixel, D3D12_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT, D3D12_COMPARISON_FUNC_LESS_EQUAL)	// 平行光源のシャドウマップ用サンプラー
		.AddSampler(2, SV_Pixel, D3D12_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT, D3D12_COMPARISON_FUNC_LESS_EQUAL,	// 点光源のシャドウマップ用サンプラー
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP)
		.Build(device->GetDevice());
	skinning_.root.AddTableParameter(0, SV_All)	// メッシュレット
		.AddTableParameter(1, SV_All)	// 頂点
		.AddTableParameter(2, SV_All)	// ユニークポインタ
		.AddTableParameter(3, SV_All)	// プリミティブインデックス
		.AddCBVParameter(0, SV_All)	// モデルの共通データ
		.AddTableParameter(4, SV_All)	//	インデックスデータ
		.AddCBVParameter(1, SV_All)	// 全体共通のデータ
		.AddCBVParameter(2, SV_All)	// カメラのView
		.AddTableParameter(5, SV_Pixel)	// マテリアル
		.AddTableParameter(6, SV_Pixel)	// 平行光源
		.AddTableParameter(7, SV_Pixel)	// 点光源
		.AddTableParameter(8, SV_Pixel, 0, lwpC::Rendering::kMaxTexture)	// テクスチャ
		.AddTableParameter(508, SV_Pixel, 0, lwpC::Shadow::Direction::kMaxCount)	// 平行光源のシャドウマップ
		.AddTableParameter(509, SV_Pixel, 0, lwpC::Shadow::Point::kMaxCount)	// 点光源のシャドウマップ
		.AddTableParameter(517, SV_All)	// スキニング用のWell
		.AddSampler(0, SV_Pixel)		// テクスチャ用サンプラー
		.AddSampler(1, SV_Pixel, D3D12_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT, D3D12_COMPARISON_FUNC_LESS_EQUAL)	// 平行光源のシャドウマップ用サンプラー
		.AddSampler(2, SV_Pixel, D3D12_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT, D3D12_COMPARISON_FUNC_LESS_EQUAL		// 点光源のシャドウマップ用サンプラー
			, D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP)
		.Build(device->GetDevice());
	static_.root.AddTableParameter(0, SV_All)	// メッシュレット
		.AddTableParameter(1, SV_All)	// 頂点
		.AddTableParameter(2, SV_All)	// ユニークポインタ
		.AddTableParameter(3, SV_All)	// プリミティブインデックス
		.AddCBVParameter(0, SV_All)	// 共通データ
		.AddCBVParameter(1, SV_All)	// カメラのView
		.AddTableParameter(4, SV_Pixel)	// マテリアル
		.AddTableParameter(5, SV_Pixel)	// 平行光源
		.AddTableParameter(6, SV_Pixel)	// 点光源
		.AddTableParameter(7, SV_Pixel, 0, lwpC::Rendering::kMaxTexture)	// テクスチャ
		.AddTableParameter(507, SV_Pixel, 0, lwpC::Shadow::Direction::kMaxCount)	// 平行光源のシャドウマップ
		.AddTableParameter(508, SV_Pixel, 0, lwpC::Shadow::Point::kMaxCount)	// 点光源のシャドウマップ
		.AddSampler(0, SV_Pixel)		// テクスチャ用サンプラー
		.AddSampler(1, SV_Pixel, D3D12_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT, D3D12_COMPARISON_FUNC_LESS_EQUAL)	// 平行光源のシャドウマップ用サンプラー
		.AddSampler(2, SV_Pixel, D3D12_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT, D3D12_COMPARISON_FUNC_LESS_EQUAL,	// 点光源のシャドウマップ用サンプラー
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP)
		.Build(device->GetDevice());

	// PSOを生成
	rigid_.pso.Init(rigid_.root, dxc, PSO::Type::Mesh)
		.SetAmpShader("ms/Meshlet.AS.hlsl")
		.SetMeshShader("ms/Meshlet.MS.hlsl")
		.SetPixelShader("ms/Meshlet.PS.hlsl")
		.Build(device->GetDevice());
	skinning_.pso.Init(skinning_.root, dxc, PSO::Type::Mesh)
		.SetAmpShader("ms/Meshlet.AS.hlsl")
		.SetMeshShader("ms/Skinning.MS.hlsl")
		.SetPixelShader("ms/Meshlet.PS.hlsl")
		.Build(device->GetDevice());
	static_.pso.Init(static_.root, dxc, PSO::Type::Mesh)
		.SetMeshShader("ms/static/Normal.MS.hlsl")
		.SetPixelShader("ms/static/Normal.PS.hlsl")
		.Build(device->GetDevice());
}

void EnvironmentMapRenderer::DrawCall(ID3D12GraphicsCommandList6* list) {
	// ビューポート
	D3D12_VIEWPORT viewport = {};
	// シャドウマップ用のテクスチャと同じサイズにする
	viewport.Width = static_cast<float>(1024);
	viewport.Height = static_cast<float>(1024);
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	// シザー矩形
	D3D12_RECT scissorRect = {};
	// 基本的にビューポートと同じ矩形が構成されるようにする
	scissorRect.left = 0;
	scissorRect.right = 1024;
	scissorRect.top = 0;
	scissorRect.bottom = 1024;

	// EMapモデル分ループする
	for (Models& m : System::engine->resourceManager_->GetModels()) {
		for (EMapModel* e : m.eMaps.ptrs.list) {
			if (!e->isActive) { continue; }

			// バリアを書き込み用に
			e->cubeMap.ChangeResourceBarrier(D3D12_RESOURCE_STATE_RENDER_TARGET, list);
			// 画面全体をクリア
			e->cubeMap.Clear(list);
			e->depthCubeMap.Clear(list);

			for (int i = 0; i < 6; i++) {
				// 描画先のRTVとDSVを設定する
				list->OMSetRenderTargets(1, &e->cubeMap.rtvInfos[i].cpuView, false, &e->depthCubeMap.dsvInfos[i].cpuView);

				// viewportを設定
				list->RSSetViewports(1, &viewport);
				// Scirssorを設定
				list->RSSetScissorRects(1, &scissorRect);

				// 描画
				DispatchAllModel(list, e->viewBuffers[i].GetGPUView());
			}

			// バリアを戻す
			e->cubeMap.ChangeResourceBarrier(D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, list);
		}
	}
}


void EnvironmentMapRenderer::Reset() {}

void EnvironmentMapRenderer::DispatchAllModel(ID3D12GraphicsCommandList6* list, D3D12_GPU_VIRTUAL_ADDRESS cameraView) {
	// 全モデル分ループ
	auto models = System::engine->resourceManager_->GetModels();
	
	// -------------------------------------------------------------- //

	// RigidModelをDispatch
	list->SetGraphicsRootSignature(rigid_.root);	// Rootセット
	for (Models& m : models) {
		// Viewをセット
		list->SetGraphicsRootConstantBufferView(7, cameraView);	// カメラ
		setViewFunction_();
		list->SetGraphicsRootDescriptorTable(11, srv_->GetFirstTexView());	// テクスチャ
		list->SetGraphicsRootDescriptorTable(12, srv_->GetFirstDirShadowView());		// 平行光源シャドウ
		list->SetGraphicsRootDescriptorTable(13, srv_->GetFirstPointShadowView());	// 点光源シャドウ

		// ModelのStructerdBufferのViewをセット
		ModelData& d = m.data;
		list->SetGraphicsRootDescriptorTable(0, d.buffers_.meshlet->GetGPUView());
		list->SetGraphicsRootDescriptorTable(1, d.buffers_.vertex->GetGPUView());
		list->SetGraphicsRootDescriptorTable(2, d.buffers_.uniqueVertexIndices->GetGPUView());
		list->SetGraphicsRootDescriptorTable(3, d.buffers_.primitiveIndices->GetGPUView());


		Models::FillMode<RigidModel, Models::RigidBuffer>& f = m.rigid;
		// Solidの描画処理
		if (!f.solid.ptrs.list.empty()) {
			// 追加のViewをセット
			list->SetGraphicsRootConstantBufferView(4, f.solid.buffer.common.GetGPUView());
			list->SetGraphicsRootDescriptorTable(5, f.solid.buffer.inst->GetGPUView());
			list->SetGraphicsRootDescriptorTable(8, f.solid.buffer.material->GetGPUView());

			list->SetPipelineState(rigid_.pso.GetState());	// PSOセット
			// メッシュレットのプリミティブ数分メッシュシェーダーを実行
			list->DispatchMesh(d.GetMeshletCount(), 1, 1);
		}
	}

	// -------------------------------------------------------------- //

	// SkinningModelをDispatch
	list->SetGraphicsRootSignature(skinning_.root);	// Rootセット
	list->SetPipelineState(skinning_.pso.GetState());	// PSOセット
	for (Models& m : models) {
		// Viewをセット
		list->SetGraphicsRootConstantBufferView(7, cameraView);	// カメラ
		setViewFunction_();
		list->SetGraphicsRootDescriptorTable(11, srv_->GetFirstTexView());	// テクスチャ
		list->SetGraphicsRootDescriptorTable(12, srv_->GetFirstDirShadowView());		// 平行光源シャドウ
		list->SetGraphicsRootDescriptorTable(13, srv_->GetFirstPointShadowView());	// 点光源シャドウ

		// ModelのStructerdBufferのViewをセット
		ModelData& d = m.data;
		list->SetGraphicsRootDescriptorTable(0, d.buffers_.meshlet->GetGPUView());
		list->SetGraphicsRootDescriptorTable(1, d.buffers_.vertex->GetGPUView());
		list->SetGraphicsRootDescriptorTable(2, d.buffers_.uniqueVertexIndices->GetGPUView());
		list->SetGraphicsRootDescriptorTable(3, d.buffers_.primitiveIndices->GetGPUView());

		Models::FillMode<SkinningModel, Models::SkinBuffer>& f = m.skin;
		// Solidの描画処理
		if (!f.solid.ptrs.list.empty()) {
			// 追加のViewをセット
			list->SetGraphicsRootConstantBufferView(4, f.solid.buffer.common.GetGPUView());
			list->SetGraphicsRootDescriptorTable(5, f.solid.buffer.inst->GetGPUView());
			list->SetGraphicsRootDescriptorTable(8, f.solid.buffer.material->GetGPUView());
			list->SetGraphicsRootDescriptorTable(14, f.solid.buffer.well->GetGPUView());	// Well


			list->SetPipelineState(skinning_.pso.GetState());	// PSOセット
			// メッシュレットのプリミティブ数分メッシュシェーダーを実行
			list->DispatchMesh(d.GetMeshletCount(), 1, 1);
		}
	}

	// -------------------------------------------------------------- //

	// StaticModelをDispatch
	list->SetGraphicsRootSignature(static_.root);	// Rootセット
	list->SetPipelineState(static_.pso.GetState());	// PSOセット
	for (Models& m : models) {
		// ModelのStructerdBufferのViewをセット
		ModelData& d = m.data;
		list->SetGraphicsRootDescriptorTable(0, d.buffers_.meshlet->GetGPUView());
		// VertexはStaticModelが持っているBufferを使う
		list->SetGraphicsRootDescriptorTable(2, d.buffers_.uniqueVertexIndices->GetGPUView());
		list->SetGraphicsRootDescriptorTable(3, d.buffers_.primitiveIndices->GetGPUView());
		buffersPtr_->SetCommonView(4, list);	// 共通
		list->SetGraphicsRootConstantBufferView(5, cameraView);	// カメラ
		list->SetGraphicsRootDescriptorTable(6, d.buffers_.materials->GetGPUView());
		buffersPtr_->SetDirLightView(7, list);	// 平行光源
		buffersPtr_->SetPointLightView(8, list);	// 点光源
		list->SetGraphicsRootDescriptorTable(9, srv_->GetFirstTexView());	// テクスチャ
		list->SetGraphicsRootDescriptorTable(10, srv_->GetFirstDirShadowView());		// 平行光源シャドウ
		list->SetGraphicsRootDescriptorTable(11, srv_->GetFirstPointShadowView());	// 点光源シャドウ

		// 全要素ループ
		for (StaticModel* ptr : m.statics.list) {
			if (!ptr->isActive) { continue; }

			// 頂点のViewをセット
			list->SetGraphicsRootDescriptorTable(1, ptr->GetVertexBufferView());
			// メッシュレット数分メッシュシェーダーを実行
			list->DispatchMesh(d.GetMeshletCount(), 1, 1);
		}
	}
}
