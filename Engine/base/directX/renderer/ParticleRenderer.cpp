#include "ParticleRenderer.h"

#include "base/ImGuiManager.h"
#include "info/FrameTracker.h"
#include "Config.h"

#include "component/Resource.h"

using namespace LWP::Base;
using namespace LWP::Object;
using namespace LWP::Resource;
using namespace LWP::Information;

void ParticleRenderer::Init(Command* cmd, std::function<void()> func) {
	cmd_ = cmd;
	setViewFunction_ = func;	// 関数セット

	// RootSignatureを生成
	root_.AddTableParameter(0, SV_All)	// メッシュレット
		.AddTableParameter(1, SV_All)	// 頂点
		.AddTableParameter(2, SV_All)	// ユニークポインタ
		.AddTableParameter(3, SV_All)	// プリミティブインデックス
		.AddCBVParameter(0, SV_All)	// モデルの共通データ
		.AddTableParameter(4, SV_All)	//	インデックスデータ
		.AddCBVParameter(1, SV_All)	// 全体共通のデータ
		.AddCBVParameter(2, SV_All)	// カメラのView
		.AddCBVParameter(3, SV_All)	// パーティクルの数
		.AddTableParameter(5, SV_All)	// パーティクルデータ
		.AddTableParameter(6, SV_Pixel)	// マテリアル
		.AddTableParameter(7, SV_Pixel)	// 平行光源
		.AddTableParameter(8, SV_Pixel)	// 点光源
		.AddTableParameter(9, SV_Pixel, 0, lwpC::Rendering::kMaxTexture)	// テクスチャ
		.AddTableParameter(509, SV_Pixel, 0, lwpC::Shadow::Direction::kMaxCount)	// 平行光源のシャドウマップ
		.AddTableParameter(510, SV_Pixel, 0, lwpC::Shadow::Point::kMaxCount)	// 点光源のシャドウマップ
		.AddSampler(0, SV_Pixel)		// テクスチャ用サンプラー
		.AddSampler(1, SV_Pixel, D3D12_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT, D3D12_COMPARISON_FUNC_LESS_EQUAL)	// 平行光源のシャドウマップ用サンプラー
		.AddSampler(2, SV_Pixel, D3D12_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT, D3D12_COMPARISON_FUNC_LESS_EQUAL,	// 点光源のシャドウマップ用サンプラー
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP)
		.Build();
	// パーティクル描画用PSOを生成
	pso_.Init(root_, PSO::Type::Mesh)
		.SetSystemAS("ms/Particle.AS.hlsl")
		.SetSystemMS("ms/Particle.MS.hlsl")
		.SetSystemPS("ms/Particle.PS.hlsl")
		.Build();
}

void ParticleRenderer::DrawCall(ID3D12GraphicsCommandList6* list) {
	// パーティクルが１つもない場合は処理を行わない
	if (particles_.size() < 0) { return; }

	ID3D12DescriptorHeap* descriptorHeaps[] = { SRV::GetInstance()->GetHeap() };
	list->SetDescriptorHeaps(1, descriptorHeaps);
	for (GPUParticle* p : particles_) {
		// 生成処理
		list->SetComputeRootSignature(*p->GetRoot());
		list->SetPipelineState(p->GetEmitterPSO()->GetState());
		list->SetComputeRootConstantBufferView(0, FrameTracker::GetInstance()->GetPreFrameBufferView());
		list->SetComputeRootConstantBufferView(1, p->GetEmitterView());
		list->SetComputeRootDescriptorTable(2, p->GetUAVDataView());
		list->Dispatch(1, 1, 1);	// 1回実行する

		// 依存関係を設定
		p->SetResourceBarrier(list);

		// 更新処理
		list->SetPipelineState(p->GetUpdatePSO()->GetState());
		list->Dispatch(1, 1, 1);	// 1回実行する

		// 描画処理（ターゲット分ループする）
		list->SetGraphicsRootSignature(root_);	// 描画用のRootとPSOに変更
		list->SetPipelineState(pso_.GetState());
		for (std::vector<Target>::iterator it = target_.begin(); it != target_.end(); ++it) {

			// 描画先のRTVとDSVを設定する
			list->OMSetRenderTargets(1, &it->back->rtvInfo.cpuView, false, &it->depth->dsvInfo.cpuView);

			// リソースバリアをセット
			D3D12_RESOURCE_STATES beforeBarrier = it->back->GetBarrier();
			it->back->ChangeResourceBarrier(D3D12_RESOURCE_STATE_RENDER_TARGET, list);

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

			// GPUパーティクル描画
			DispatchAllParticle(list, p, it->view);

			// バリアを元に戻す
			it->back->ChangeResourceBarrier(beforeBarrier, list);
		}
	}
	
	// 更新処理
	// 当たり判定
	// ヒット処理

}

void ParticleRenderer::Reset() {
	target_.clear();
	for (GPUParticle* p : particles_) {
		// エミッターの生成フラグは初期化しておく
		p->SetResetEmitterFlag();
	}
	particles_.clear();
}

void ParticleRenderer::DispatchAllParticle(ID3D12GraphicsCommandList6* list, GPUParticle* p, D3D12_GPU_VIRTUAL_ADDRESS cameraView) {
	SRV* srv = SRV::GetInstance();

	// -------------------------------------------------------------- //

	// Viewをセット
	list->SetGraphicsRootConstantBufferView(7, cameraView);	// カメラ
	setViewFunction_();
	list->SetGraphicsRootDescriptorTable(13, srv->GetFirstTexView());	// テクスチャ
	list->SetGraphicsRootDescriptorTable(14, srv->GetFirstDirShadowView());		// 平行光源シャドウ
	list->SetGraphicsRootDescriptorTable(15, srv->GetFirstPointShadowView());	// 点光源シャドウ

	// ModelのStructerdBufferのViewをセット
	ModelData* data = p->model.GetModelData();
	list->SetGraphicsRootDescriptorTable(0, data->buffers_.meshlet->GetGPUView());
	list->SetGraphicsRootDescriptorTable(1, data->buffers_.vertex->GetGPUView());
	list->SetGraphicsRootDescriptorTable(2, data->buffers_.uniqueVertexIndices->GetGPUView());
	list->SetGraphicsRootDescriptorTable(3, data->buffers_.primitiveIndices->GetGPUView());

	// 追加のViewをセット
	list->SetGraphicsRootConstantBufferView(8, p->GetCountView());
	list->SetGraphicsRootDescriptorTable(9, p->GetSRVDataView());
	list->SetGraphicsRootDescriptorTable(10, data->buffers_.materials->GetGPUView());
	//list->SetGraphicsRootDescriptorTable(5, f.solid.buffer.inst->GetGPUView());
	//list->SetGraphicsRootDescriptorTable(8, f.solid.buffer.material->GetGPUView());

	// メッシュレットのプリミティブ数分メッシュシェーダーを実行
	list->DispatchMesh(data->GetMeshletCount(), 1, 1);
}
