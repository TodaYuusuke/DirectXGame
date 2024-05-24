#include "MeshRenderer.h"

#include "base/ImGuiManager.h"
#include "Config.h"

#include "component/Resource.h"

using namespace LWP::Base;
using namespace LWP::Resource;

void MeshRenderer::Init(GPUDevice* device, SRV* srv, DXC* dxc, std::function<void()> func) {
	srv_ = srv;
	setViewFunction_ = func;	// 関数セット

	// RootSignatureを生成
	rigid_.root.AddTableParameter(0, SV_All)	// メッシュレット
		.AddTableParameter(1, SV_All)	// 頂点
		.AddTableParameter(2, SV_All)	// ユニークポインタ
		.AddTableParameter(3, SV_All)	// プリミティブインデックス
		.AddCBVParameter(2, SV_All)	//	インデックスデータ（複数描画時にすり替わる部分）
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
		.AddSampler(2, SV_Pixel, D3D12_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT, D3D12_COMPARISON_FUNC_LESS_EQUAL		// 点光源のシャドウマップ用サンプラー
			, D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP)
		.Build(device->GetDevice());
	skinning_.root.AddTableParameter(0, SV_All)	// メッシュレット
		.AddTableParameter(1, SV_All)	// 頂点
		.AddTableParameter(2, SV_All)	// ユニークポインタ
		.AddTableParameter(3, SV_All)	// プリミティブインデックス
		.AddCBVParameter(2, SV_All)	//	インデックスデータ（複数描画時にすり替わる部分）
		.AddCBVParameter(0, SV_All)	// 共通データ
		.AddCBVParameter(1, SV_All)	// カメラのView
		.AddTableParameter(4, SV_Pixel)	// マテリアル
		.AddTableParameter(5, SV_Pixel)	// 平行光源
		.AddTableParameter(6, SV_Pixel)	// 点光源
		.AddTableParameter(7, SV_Pixel, 0, lwpC::Rendering::kMaxTexture)	// テクスチャ
		.AddTableParameter(507, SV_Pixel, 0, lwpC::Shadow::Direction::kMaxCount)	// 平行光源のシャドウマップ
		.AddTableParameter(508, SV_Pixel, 0, lwpC::Shadow::Point::kMaxCount)	// 点光源のシャドウマップ
		.AddTableParameter(516, SV_All)	// スキニング用のWell
		.AddSampler(0, SV_Pixel)		// テクスチャ用サンプラー
		.AddSampler(1, SV_Pixel, D3D12_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT, D3D12_COMPARISON_FUNC_LESS_EQUAL)	// 平行光源のシャドウマップ用サンプラー
		.AddSampler(2, SV_Pixel, D3D12_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT, D3D12_COMPARISON_FUNC_LESS_EQUAL		// 点光源のシャドウマップ用サンプラー
			, D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP)
		.Build(device->GetDevice());

	// PSOを生成
	rigid_.pso.Init(rigid_.root, dxc, PSO::Type::Mesh)
#if DEMO
		.SetMeshShader("ms/Meshlet.MS.hlsl")
		//.SetMeshShader("ms/MeshletDebug.MS.hlsl")
#else
		.SetMeshShader("ms/Meshlet.MS.hlsl")
#endif
		.SetPixelShader("ms/Meshlet.PS.hlsl")
		.Build(device->GetDevice());
	skinning_.pso.Init(skinning_.root, dxc, PSO::Type::Mesh)
		.SetMeshShader("ms/Skinning.MS.hlsl")
		.SetPixelShader("ms/Meshlet.PS.hlsl")
		.Build(device->GetDevice());


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
		it->depth->Clear(list);

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

void MeshRenderer::Reset() {
	target_.clear();
}

void MeshRenderer::DispatchAllModel(ID3D12GraphicsCommandList6* list, D3D12_GPU_VIRTUAL_ADDRESS cameraView) {
	// 全モデル分ループ
	auto models = System::engine->resourceManager_->GetModels();

	// RigidModelをDispatch
	list->SetGraphicsRootSignature(rigid_.root);	// Rootセット
	list->SetPipelineState(rigid_.pso.GetState());	// PSOセット
	for (Models& m : models) {
		// Viewをセット
		list->SetGraphicsRootConstantBufferView(6, cameraView);	// カメラ
		setViewFunction_();
		list->SetGraphicsRootDescriptorTable(10, srv_->GetFirstTexView());	// テクスチャ
		list->SetGraphicsRootDescriptorTable(11, srv_->GetFirstDirShadowView());		// 平行光源シャドウ
		list->SetGraphicsRootDescriptorTable(12, srv_->GetFirstPointShadowView());	// 点光源シャドウ

		// ModelのStructerdBufferのViewをセット
		ModelData& d = m.data;
		list->SetGraphicsRootDescriptorTable(0, d.buffers_.meshlet->GetGPUView());
		list->SetGraphicsRootDescriptorTable(1, d.buffers_.vertex->GetGPUView());
		list->SetGraphicsRootDescriptorTable(2, d.buffers_.uniqueVertexIndices->GetGPUView());
		list->SetGraphicsRootDescriptorTable(3, d.buffers_.primitiveIndices->GetGPUView());
		list->SetGraphicsRootDescriptorTable(7, d.buffers_.materials->GetGPUView());

		// リキッドモデルを描画
		for (RigidModel* rm : m.rigid.list) {
			// isActiveがfalseなら描画しない
			if (!rm->isActive) { continue; }

			// ConstantBufferのViewをセット
			list->SetGraphicsRootConstantBufferView(4, rm->buffer.GetGPUView());

			// メッシュレットのプリミティブ数分メッシュシェーダーを実行
			list->DispatchMesh(d.GetMeshletCount(), 1, 1);
		}
	}

	// SkinningModelをDispatch
	list->SetGraphicsRootSignature(skinning_.root);	// Rootセット
	list->SetPipelineState(skinning_.pso.GetState());	// PSOセット
	for (Models& m : models) {
		// Viewをセット
		list->SetGraphicsRootConstantBufferView(6, cameraView);	// カメラ
		setViewFunction_();
		list->SetGraphicsRootDescriptorTable(10, srv_->GetFirstTexView());	// テクスチャ
		list->SetGraphicsRootDescriptorTable(11, srv_->GetFirstDirShadowView());		// 平行光源シャドウ
		list->SetGraphicsRootDescriptorTable(12, srv_->GetFirstPointShadowView());	// 点光源シャドウ

		// ModelのStructerdBufferのViewをセット
		ModelData& d = m.data;
		list->SetGraphicsRootDescriptorTable(0, d.buffers_.meshlet->GetGPUView());
		list->SetGraphicsRootDescriptorTable(1, d.buffers_.vertex->GetGPUView());
		list->SetGraphicsRootDescriptorTable(2, d.buffers_.uniqueVertexIndices->GetGPUView());
		list->SetGraphicsRootDescriptorTable(3, d.buffers_.primitiveIndices->GetGPUView());
		list->SetGraphicsRootDescriptorTable(7, d.buffers_.materials->GetGPUView());

		// スキニングモデルを描画
		for (SkinningModel* sm : m.skin.list) {
			// isActiveがfalseなら描画しない
			if (!sm->isActive) { continue; }

			// ConstantBufferのViewをセット
			list->SetGraphicsRootConstantBufferView(4, sm->buffer.GetGPUView());
			// Wellをセット
			list->SetGraphicsRootDescriptorTable(13, sm->wellBuffer->GetGPUView());

			// メッシュレットのプリミティブ数分メッシュシェーダーを実行
			list->DispatchMesh(d.GetMeshletCount(), 1, 1);
		}
	}


}
