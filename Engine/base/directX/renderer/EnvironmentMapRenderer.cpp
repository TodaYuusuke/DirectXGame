#include "EnvironmentMapRenderer.h"

#include "component/Resource.h"
#include "resources/model/RigidModel.h"
#include "Config.h"


using namespace LWP::Base;
using namespace LWP::Resource;

// サイズをここで指定
EnvironmentMapRenderer::EnvironmentMapRenderer() {}

void EnvironmentMapRenderer::Init(GPUDevice* device, SRV* srv, DXC* dxc, std::function<void()> func) {
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

void ShadowRenderer::DrawCall(ID3D12GraphicsCommandList6* list) {
	// ターゲット分ループする（平行光源）
	for (const Target<SM_Direction>& t : targetDir_) {
		// 描画先のDSVを設定する
		list->OMSetRenderTargets(0, nullptr, false, &t.shadow->dsvInfo.cpuView);

		// バリアを書き込み用に
		t.shadow->ChangeResourceBarrier(D3D12_RESOURCE_STATE_DEPTH_WRITE, list);
		// 画面全体をクリア
		t.shadow->Clear(list);

		// ビューポート
		D3D12_VIEWPORT viewport = {};
		// シャドウマップ用のテクスチャと同じサイズにする
		viewport.Width = static_cast<float>(t.shadow->width);
		viewport.Height = static_cast<float>(t.shadow->height);
		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		// viewportを設定
		list->RSSetViewports(1, &viewport);

		// シザー矩形
		D3D12_RECT scissorRect = {};
		// 基本的にビューポートと同じ矩形が構成されるようにする
		scissorRect.left = 0;
		scissorRect.right = t.shadow->width;
		scissorRect.top = 0;
		scissorRect.bottom = t.shadow->height;
		// Scirssorを設定
		list->RSSetScissorRects(1, &scissorRect);

		// 描画
		DispatchAllModel(list, t.view);

		// バリアを戻す
		t.shadow->ChangeResourceBarrier(D3D12_RESOURCE_STATE_GENERIC_READ, list);
	}

	// ターゲット分ループする（ポイントライト）
	for (const TargetPoint& t : targetPoint_) {
		// バリアを書き込み用に
		t.shadow->ChangeResourceBarrier(D3D12_RESOURCE_STATE_DEPTH_WRITE, list);
		// 画面全体をクリア
		t.shadow->Clear(list);

		// ビューポート
		D3D12_VIEWPORT viewport = {};
		// シャドウマップ用のテクスチャと同じサイズにする
		viewport.Width = static_cast<float>(t.shadow->width);
		viewport.Height = static_cast<float>(t.shadow->height);
		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;

		// シザー矩形
		D3D12_RECT scissorRect = {};
		// 基本的にビューポートと同じ矩形が構成されるようにする
		scissorRect.left = 0;
		scissorRect.right = t.shadow->width;
		scissorRect.top = 0;
		scissorRect.bottom = t.shadow->height;

		// ポイントライトなので6回レンダリング
		D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle[6] = {
			t.shadow->dsvInfo.cpuView,
			t.shadow->dsvInfos[0].cpuView,
			t.shadow->dsvInfos[1].cpuView,
			t.shadow->dsvInfos[2].cpuView,
			t.shadow->dsvInfos[3].cpuView,
			t.shadow->dsvInfos[4].cpuView
		};
		for (int i = 0; i < 6; i++) {
			// 描画先のDSVを設定する
			list->OMSetRenderTargets(0, nullptr, false, &dsvHandle[i]);

			// viewportを設定
			list->RSSetViewports(1, &viewport);
			// Scirssorを設定
			list->RSSetScissorRects(1, &scissorRect);
			// 描画
			DispatchAllModel(list, t.views[i]);
		}

		// バリアを戻す
		t.shadow->ChangeResourceBarrier(D3D12_RESOURCE_STATE_GENERIC_READ, list);
	}
}


void ShadowRenderer::Reset() {
	targetDir_.clear();
	targetPoint_.clear();
	
	indexBuffer_.Reset();
}

void ShadowRenderer::DispatchAllModel(ID3D12GraphicsCommandList6* list, D3D12_GPU_VIRTUAL_ADDRESS view) {

	// ** 従来のVertexShader ** //

	// RootSignatureを設定
	list->SetGraphicsRootSignature(normal_.root);
	// PSOを設定
	list->SetPipelineState(normal_.pso.GetState());
	// 視点のViewをセット
	list->SetGraphicsRootConstantBufferView(1, view);
	setViewFunction_();
	// ディスクリプタテーブルを登録
	list->SetGraphicsRootDescriptorTable(0, indexBuffer_.GetGPUView());
	// 全三角形を１つのDrawCallで描画
	list->DrawInstanced(3, indexBuffer_.GetCount() / 3, 0, 0);


	// ** MeshShader ** //

	// 全モデル分ループ
	auto models = System::engine->resourceManager_->GetModels();

	// RigidModelをDispatch
	list->SetGraphicsRootSignature(rigid_.root);	// RootSignatureを設定
	list->SetPipelineState(rigid_.pso.GetState());	// PSOを設定
	for (Models& m : models) {
		// Viewをセット

		// ModelのStructerdBufferのViewをセット
		ModelData& d = m.data;
		// 視点のViewをセット
		list->SetGraphicsRootConstantBufferView(6, view);
		// ModelのStructerdBufferのViewをセット
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

			// メッシュレットのプリミティブ数分メッシュシェーダーを実行
			list->DispatchMesh(d.GetMeshletCount(), 1, 1);
		}
	}

	// SkinModelをDispatch
	list->SetGraphicsRootSignature(skinning_.root);	// RootSignatureを設定
	list->SetPipelineState(skinning_.pso.GetState());	// PSOを設定
	for (Models& m : models) {
		// Viewをセット

		// ModelのStructerdBufferのViewをセット
		ModelData& d = m.data;
		// 視点のViewをセット
		list->SetGraphicsRootConstantBufferView(6, view);
		// ModelのStructerdBufferのViewをセット
		list->SetGraphicsRootDescriptorTable(0, d.buffers_.meshlet->GetGPUView());
		list->SetGraphicsRootDescriptorTable(1, d.buffers_.vertex->GetGPUView());
		list->SetGraphicsRootDescriptorTable(2, d.buffers_.uniqueVertexIndices->GetGPUView());
		list->SetGraphicsRootDescriptorTable(3, d.buffers_.primitiveIndices->GetGPUView());
		// WellのBufferをセット
		//list->SetGraphicsRootDescriptorTable(6, sm->wellBuffer->GetGPUView());


		Models::FillMode<SkinningModel, Models::SkinBuffer>& f = m.skin;
		// Solidの描画処理
		if (!f.solid.ptrs.list.empty()) {
			// 追加のViewをセット
			list->SetGraphicsRootConstantBufferView(4, f.solid.buffer.common.GetGPUView());
			list->SetGraphicsRootDescriptorTable(5, f.solid.buffer.inst->GetGPUView());
			list->SetGraphicsRootDescriptorTable(7, f.solid.buffer.well->GetGPUView());	// Well

			list->SetPipelineState(skinning_.pso.GetState());	// PSOセット
			// メッシュレットのプリミティブ数分メッシュシェーダーを実行
			list->DispatchMesh(d.GetMeshletCount(), 1, 1);
		}
		
		// リキッドモデルを描画
		//for (SkinningModel* sm : m.skin.list) {
		//	// isActiveがfalseもしくはLightingがfalseなら描画しない
		//	if (!sm->isActive || !sm->enableLighting) { continue; }
		//	// メッシュレットのプリミティブ数分メッシュシェーダーを実行
		//	list->DispatchMesh(d.GetMeshletCount(), 1, 1);
		//}
	}
}
