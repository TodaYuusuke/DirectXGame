#include "ShadowRenderer.h"

#include "component/Resource.h"
#include "resources/model/RigidModel.h"
#include "Config.h"


using namespace LWP::Base;
using namespace LWP::Resource;

// サイズをここで指定
ShadowRenderer::ShadowRenderer() : indexBuffer_(lwpC::Rendering::kMaxIndex) {}

void ShadowRenderer::Init(std::function<void()> func) {
	// StructuredBufferを初期化
	indexBuffer_.Init();
	// RootSignatureを生成
	normal_.root.AddTableParameter(0, SV_Vertex)	// インデックスのデータ
		.AddCBVParameter(0, SV_Vertex)		// 描画に使うViewprojection
		.AddTableParameter(1, SV_Vertex)	// 頂点データ
		.AddTableParameter(2, SV_Vertex)	// トランスフォーム
		.Build();
	rigid_.root.AddTableParameter(0, SV_All)	// メッシュレット
		.AddTableParameter(1, SV_All)	// 頂点
		.AddTableParameter(2, SV_All)	// ユニークポインタ
		.AddTableParameter(3, SV_All)	// プリミティブインデックス
		.AddCBVParameter(0, SV_All)	// モデル共通データ
		.AddTableParameter(4, SV_All)	// インデックス
		.AddCBVParameter(1, SV_All)	// ビュープロジェクション
		.Build();
	skinning_.root.AddTableParameter(0, SV_All)	// メッシュレット
		.AddTableParameter(1, SV_All)	// 頂点
		.AddTableParameter(2, SV_All)	// ユニークポインタ
		.AddTableParameter(3, SV_All)	// プリミティブインデックス
		.AddCBVParameter(0, SV_All)	// モデル共通データ
		.AddTableParameter(4, SV_All)	// インデックス
		.AddCBVParameter(1, SV_All)	// ビュープロジェクション
		.AddTableParameter(5, SV_All)	// スキニング用のWell
		.Build();

	// PSOを生成
	normal_.pso.Init(normal_.root)
		.SetSystemVS("ShadowMap.VS.hlsl")
		.SetRasterizerState(D3D12_CULL_MODE_FRONT, D3D12_FILL_MODE_SOLID)
		.SetDSVFormat(DXGI_FORMAT_D32_FLOAT)
		.Build();
	rigid_.pso.Init(rigid_.root, PSO::Type::Mesh)
		.SetSystemAS("ms/Meshlet.AS.hlsl")
		.SetSystemMS("ms/ShadowMap.MS.hlsl")
		.SetRasterizerState(D3D12_CULL_MODE_FRONT, D3D12_FILL_MODE_SOLID)
		.SetDSVFormat(DXGI_FORMAT_D32_FLOAT)
		.Build();
	skinning_.pso.Init(skinning_.root, PSO::Type::Mesh)
		.SetSystemAS("ms/Meshlet.AS.hlsl")
		.SetSystemMS("ms/SkinningShadowMap.MS.hlsl")
		.SetRasterizerState(D3D12_CULL_MODE_FRONT, D3D12_FILL_MODE_SOLID)
		.SetDSVFormat(DXGI_FORMAT_D32_FLOAT)
		.Build();

	// 関数セット
	setViewFunction_ = func;
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
	auto models = Resource::Manager::GetInstance()->GetModels();

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
