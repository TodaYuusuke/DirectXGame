#include "MeshRenderer.h"

#include "base/ImGuiManager.h"
#include "Config.h"

#include "component/Resource.h"

using namespace LWP::Base;
using namespace LWP::Resource;

void MeshRenderer::Init(Command* cmd, std::function<void()> func) {
	cmd_ = cmd;
	setViewFunction_ = func;	// 関数セット

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
		.Build();
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
		.Build();
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
		.Build();
	// eMap
	eMap_.root.AddTableParameter(0, SV_All)	// メッシュレット
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
		.AddTableParameter(517, SV_Pixel)	// 環境マップ
		.AddSampler(0, SV_Pixel)		// テクスチャ用サンプラー
		.AddSampler(1, SV_Pixel, D3D12_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT, D3D12_COMPARISON_FUNC_LESS_EQUAL)	// 平行光源のシャドウマップ用サンプラー
		.AddSampler(2, SV_Pixel, D3D12_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT, D3D12_COMPARISON_FUNC_LESS_EQUAL,	// 点光源のシャドウマップ用サンプラー
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP)
		.Build();

	// PSOを生成
	rigid_.pso.Init(rigid_.root, PSO::Type::Mesh)
		.SetSystemAS("ms/Meshlet.AS.hlsl")
		.SetSystemMS("ms/Meshlet.MS.hlsl")
		.SetSystemPS("ms/Meshlet.PS.hlsl")
		.Build();
	rigid_.wirePso.Init(rigid_.root, PSO::Type::Mesh)
		.SetRasterizerState(D3D12_CULL_MODE_NONE, D3D12_FILL_MODE_WIREFRAME)
		.SetSystemAS("ms/Meshlet.AS.hlsl")
		.SetSystemMS("ms/Meshlet.MS.hlsl")
		.SetSystemPS("ms/Meshlet.PS.hlsl")
		.Build();
	skinning_.pso.Init(skinning_.root, PSO::Type::Mesh)
		.SetSystemAS("ms/Meshlet.AS.hlsl")
		.SetSystemMS("ms/Skinning.MS.hlsl")
		.SetSystemPS("ms/Meshlet.PS.hlsl")
		.Build();
	skinning_.wirePso.Init(skinning_.root, PSO::Type::Mesh)
		.SetRasterizerState(D3D12_CULL_MODE_NONE, D3D12_FILL_MODE_WIREFRAME)
		.SetSystemAS("ms/Meshlet.AS.hlsl")
		.SetSystemMS("ms/Skinning.MS.hlsl")
		.SetSystemPS("ms/Meshlet.PS.hlsl")
		.Build();
	static_.pso.Init(static_.root, PSO::Type::Mesh)
		.SetSystemMS("ms/static/Normal.MS.hlsl")
		.SetSystemPS("ms/static/Normal.PS.hlsl")
		.Build();
	static_.wirePso.Init(static_.root, PSO::Type::Mesh)
		.SetRasterizerState(D3D12_CULL_MODE_NONE, D3D12_FILL_MODE_WIREFRAME)
		.SetSystemMS("ms/static/Normal.MS.hlsl")
		.SetSystemPS("ms/static/Normal.PS.hlsl")
		.Build();
	eMap_.pso.Init(eMap_.root, PSO::Type::Mesh)
		.SetSystemAS("ms/eMap/Meshlet.AS.hlsl")
		.SetSystemMS("ms/eMap/Meshlet.MS.hlsl")
		.SetSystemPS("ms/eMap/EMap.PS.hlsl")
		.Build();


	// 草用のRootSignatureとPSOを生成
	grassData_.generate.root.AddCBVParameter(0, SV_All)
		.AddUAVParameter(0, SV_All)	// 書き込むリソース
		.AddTableParameter(0, SV_All)	// 地形のテクスチャ
		.AddSampler(0, SV_All)		// テクスチャ用サンプラー
		.Build();
	grassData_.generate.pso.Init(grassData_.generate.root, PSO::Type::Compute)
		.SetSystemCS("cs/Grass.CS.hlsl")
		.Build();
	grassData_.generate.cBuffer.Init();
	grassData_.generate.rwBuffer = std::make_unique<RWStructuredBuffer<Math::Vector3>>(grassData_.generate.kSize);
	grassData_.generate.rwBuffer->Init();

	
	grassData_.root.AddCBVParameter(0, SV_All)	// カメラデータ
		.AddTableParameter(0, SV_All)	// 草を生やす座標データ
		.Build();
	grassData_.pso.Init(grassData_.root, PSO::Type::Mesh)
		.SetSystemMS("ms/terrain/TerrainGrass.MS.hlsl")
		.SetSystemPS("ms/terrain/TerrainGrass.PS.hlsl")
		.SetRasterizerState(D3D12_CULL_MODE_NONE)
		.Build();

}

void MeshRenderer::DrawCall(ID3D12GraphicsCommandList6* list) {
	
	// ターゲット分ループする
	for (std::vector<Target>::iterator it = target_.begin(); it != target_.end(); ++it) {
		// 描画先のRTVとDSVを設定する
		list->OMSetRenderTargets(1, &it->back->rtvInfo.cpuView, false, &it->depth->dsvInfo.cpuView);
		
		// リソースバリアをセット
		D3D12_RESOURCE_STATES beforeBarrier = it->back->GetBarrier();
		it->back->ChangeResourceBarrier(D3D12_RESOURCE_STATE_RENDER_TARGET, list);
		// 全画面クリア
		it->back->Clear(list);
		// 指定した深度で画面全体をクリアする
		it->depth->ClearDepth(list);

		// ビューポート
		D3D12_VIEWPORT viewport = {};
		// クライアント領域のサイズと一緒にして画面全体に表示
		viewport.Width = static_cast<float>(it->back->width);
		viewport.Height = static_cast<float>(it->back->height);
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
		scissorRect.right = it->back->width;
		scissorRect.top = 0;
		scissorRect.bottom = it->back->height;
		// Scirssorを設定
		list->RSSetScissorRects(1, &scissorRect);

		// 全モデルを描画
		DispatchAllModel(list, it->view);

		// バリアを元に戻す
		it->back->ChangeResourceBarrier(beforeBarrier, list);
	}
}

void MeshRenderer::GenerateGrass(Math::Vector3 min, Math::Vector3 max, int textureIndex) {
	grassData_.generate.cBuffer.data_->min = min;
	grassData_.generate.cBuffer.data_->max = max;
	SRV* srv = SRV::GetInstance();

	// 草の地点を生成するCSをコマンドリストに積む
	ID3D12GraphicsCommandList6* list = cmd_->List();
	list->SetComputeRootSignature(grassData_.generate.root);
	list->SetPipelineState(grassData_.generate.pso.GetState());
	ID3D12DescriptorHeap* descriptorHeaps[] = { srv->GetHeap() };
	list->SetDescriptorHeaps(1, descriptorHeaps);
	list->SetComputeRootConstantBufferView(0, grassData_.generate.cBuffer.GetGPUView());
	list->SetComputeRootDescriptorTable(1, grassData_.generate.rwBuffer->GetUAVGPUView());
	list->SetComputeRootDescriptorTable(2, srv->GetGPUHandle(textureIndex));
	list->Dispatch(grassData_.generate.kSize, 1, 1);	// 65535回生成する

	grassData_.generate.generated = true;
}


void MeshRenderer::Reset() {
	target_.clear();
}

void MeshRenderer::DispatchAllModel(ID3D12GraphicsCommandList6* list, D3D12_GPU_VIRTUAL_ADDRESS cameraView) {
	// 全モデル分ループ
	auto models = Resource::Manager::GetInstance()->GetModels();
	SRV* srv = SRV::GetInstance();

	// -------------------------------------------------------------- //

	// RigidModelをDispatch
	list->SetGraphicsRootSignature(rigid_.root);	// Rootセット
	for (Models& m : models) {
		// Viewをセット
		list->SetGraphicsRootConstantBufferView(7, cameraView);	// カメラ
		setViewFunction_();
		list->SetGraphicsRootDescriptorTable(11, srv->GetFirstTexView());	// テクスチャ
		list->SetGraphicsRootDescriptorTable(12, srv->GetFirstDirShadowView());		// 平行光源シャドウ
		list->SetGraphicsRootDescriptorTable(13, srv->GetFirstPointShadowView());	// 点光源シャドウ

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
		// WireFrameの描画処理
		if (!f.wireFrame.ptrs.list.empty()) {
			// 追加のViewをセット
			list->SetGraphicsRootConstantBufferView(4, f.wireFrame.buffer.common.GetGPUView());
			list->SetGraphicsRootDescriptorTable(5, f.wireFrame.buffer.inst->GetGPUView());
			list->SetGraphicsRootDescriptorTable(8, f.wireFrame.buffer.material->GetGPUView());

			list->SetPipelineState(rigid_.wirePso.GetState());	// PSOセット
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
		list->SetGraphicsRootDescriptorTable(11, srv->GetFirstTexView());	// テクスチャ
		list->SetGraphicsRootDescriptorTable(12, srv->GetFirstDirShadowView());		// 平行光源シャドウ
		list->SetGraphicsRootDescriptorTable(13, srv->GetFirstPointShadowView());	// 点光源シャドウ

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
		// WireFrameの描画処理
		if (!f.wireFrame.ptrs.list.empty()) {
			// 追加のViewをセット
			list->SetGraphicsRootConstantBufferView(4, f.wireFrame.buffer.common.GetGPUView());
			list->SetGraphicsRootDescriptorTable(5, f.wireFrame.buffer.inst->GetGPUView());
			list->SetGraphicsRootDescriptorTable(8, f.wireFrame.buffer.material->GetGPUView());
			list->SetGraphicsRootDescriptorTable(14, f.solid.buffer.well->GetGPUView());	// Well

			list->SetPipelineState(skinning_.wirePso.GetState());	// PSOセット
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
		list->SetGraphicsRootDescriptorTable(9, srv->GetFirstTexView());	// テクスチャ
		list->SetGraphicsRootDescriptorTable(10, srv->GetFirstDirShadowView());		// 平行光源シャドウ
		list->SetGraphicsRootDescriptorTable(11, srv->GetFirstPointShadowView());	// 点光源シャドウ

		// 全要素ループ
		for (StaticModel* ptr : m.statics.list) {
			if (!ptr->isActive) { continue; }

			// 頂点のViewをセット
			list->SetGraphicsRootDescriptorTable(1, ptr->GetVertexBufferView());
			// メッシュレット数分メッシュシェーダーを実行
			list->DispatchMesh(d.GetMeshletCount(), 1, 1);
		}
	}

	// -------------------------------------------------------------- //

	// EMapModelをDispatch
	list->SetGraphicsRootSignature(eMap_.root);	// Rootセット
	list->SetPipelineState(eMap_.pso.GetState());	// PSOセット
	for (Models& m : models) {
		// Viewをセット
		list->SetGraphicsRootConstantBufferView(7, cameraView);	// カメラ
		setViewFunction_();
		list->SetGraphicsRootDescriptorTable(11, srv->GetFirstTexView());	// テクスチャ
		list->SetGraphicsRootDescriptorTable(12, srv->GetFirstDirShadowView());		// 平行光源シャドウ
		list->SetGraphicsRootDescriptorTable(13, srv->GetFirstPointShadowView());	// 点光源シャドウ

		// ModelのStructerdBufferのViewをセット
		ModelData& d = m.data;
		list->SetGraphicsRootDescriptorTable(0, d.buffers_.meshlet->GetGPUView());
		list->SetGraphicsRootDescriptorTable(1, d.buffers_.vertex->GetGPUView());
		list->SetGraphicsRootDescriptorTable(2, d.buffers_.uniqueVertexIndices->GetGPUView());
		list->SetGraphicsRootDescriptorTable(3, d.buffers_.primitiveIndices->GetGPUView());

		// 全要素ループ
		Models::Pointers<EMapModel, Models::RigidBuffer>& e = m.eMaps;
		for (EMapModel* ptr : e.ptrs.list) {
			if (!ptr->isActive) { continue; }

			// 追加のViewをセット
			list->SetGraphicsRootConstantBufferView(4, e.buffer.common.GetGPUView());
			list->SetGraphicsRootDescriptorTable(5, e.buffer.inst->GetGPUView());
			list->SetGraphicsRootDescriptorTable(8, e.buffer.material->GetGPUView());
			list->SetGraphicsRootDescriptorTable(14, ptr->cubeMap.GetSRVGPUView());

			// メッシュレット数分メッシュシェーダーを実行
			list->DispatchMesh(d.GetMeshletCount(), 1, 1);
		}
	}

	// -------------------------------------------------------------- //

	if (grassData_.generate.generated) {
		// 草をDispatch
		list->SetGraphicsRootSignature(grassData_.root);	// Rootセット
		list->SetPipelineState(grassData_.pso.GetState());	// PSOセット
		// Viewをセット
		list->SetGraphicsRootConstantBufferView(0, cameraView);
		list->SetGraphicsRootDescriptorTable(1, grassData_.generate.rwBuffer->GetSRVGPUView());
		// 生やす地点数分メッシュシェーダーを実行
		list->DispatchMesh(grassData_.generate.kSize, 1, 1);
	}
}
