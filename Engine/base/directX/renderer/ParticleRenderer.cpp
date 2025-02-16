#include "ParticleRenderer.h"

#include "base/ImGuiManager.h"
#include "info/FrameTracker.h"
#include "Config.h"

#include "component/Resource.h"

using namespace LWP::Base;
using namespace LWP::Object;
using namespace LWP::Resource;
using namespace LWP::Information;
using namespace LWP::Utility;

void ParticleRenderer::Init(Command* cmd, std::function<void()> func) {
	cmd_ = cmd;
	setViewFunction_ = func;	// 関数セット

	// 初期化用RootSignatureを生成
	initShader_.root.AddCBVParameter(0, SV_All)
		.AddUAVParameter(0, SV_All)	// パーティクルデータ
		.AddUAVParameter(1, SV_All)	// FreeListIndex
		.AddUAVParameter(2, SV_All)	// FreeList
		.Build();
	// 初期化用PSOを生成
	initShader_.pso.Init(initShader_.root, PSO::Type::Compute)
		.SetSystemCS("ms/particle/ParticleInit.CS.hlsl")
		.Build();
	// 初期化用RootSignatureを生成
	initHitShader_.root.AddCBVParameter(0, SV_All)
		.AddUAVParameter(0, SV_All)	// ヒットリスト
		.Build();
	// 初期化用PSOを生成
	initHitShader_.pso.Init(initHitShader_.root, PSO::Type::Compute)
		.SetSystemCS("ms/particle/ResultInit.CS.hlsl")
		.Build();

	// 描画用RootSignatureを生成
	renderingShader_.root.AddTableParameter(0, SV_All)	// メッシュレット
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
	// 描画用PSOを生成
	renderingShader_.pso.Init(renderingShader_.root, PSO::Type::Mesh)
		.SetSystemAS("ms/particle/Particle.AS.hlsl")
		.SetSystemMS("ms/particle/Particle.MS.hlsl")
		.SetSystemPS("ms/particle/Particle.PS.hlsl")
		.Build();

	// 当たり判定用のPSOたち
	collider_.wireFrame.Copy(renderingShader_.pso)	// PSOをコピー
		.SetRTVFormat(DXGI_FORMAT_R32_SINT)
		.SetBlendState(false)
		.SetRasterizerState(D3D12_CULL_MODE_NONE, D3D12_FILL_MODE_WIREFRAME)	// ワイヤーフレームに
		.SetSystemMS("ms/particle/ParticleCollider.MS.hlsl")
		.SetSystemPS("ms/particle/ParticleCollider.PS.hlsl")
		.Build();
	collider_.frontFaceRoot.AddTableParameter(0, SV_All)	// メッシュレット
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
	collider_.frontFacePSO.Init(collider_.frontFaceRoot, PSO::Type::Mesh)	// パーティクル以外のモデルを描画するためのPSO
		.SetRasterizerState(D3D12_CULL_MODE_BACK)	// 表だけ描画
		.SetDepthState(true,
			D3D12_DEPTH_WRITE_MASK_ZERO,	// 深度書き込み禁止
			D3D12_COMPARISON_FUNC_LESS_EQUAL)	// 深度が近いものだけをパス
		.SetStencilState(true,
			D3D12_DEPTH_STENCILOP_DESC(
				D3D12_STENCIL_OP_KEEP,	// ステンシルテスト失敗時、値を保持
				D3D12_STENCIL_OP_KEEP,	// 深度テスト失敗時、値を保持
				D3D12_STENCIL_OP_INCR_SAT,	// 深度・ステンシルテスト両方成功時、ステンシル値を加算
				D3D12_COMPARISON_FUNC_ALWAYS),	// 常にステンシルテストを成功させる
			D3D12_DEPTH_STENCILOP_DESC(
				D3D12_STENCIL_OP_KEEP,	// ステンシルテスト失敗時、値を保持
				D3D12_STENCIL_OP_KEEP,	// 深度テスト失敗時、値を保持
				D3D12_STENCIL_OP_DECR_SAT,	// 深度・ステンシルテスト両方成功時、ステンシル値を減算
				D3D12_COMPARISON_FUNC_ALWAYS))	// 常にステンシルテストを成功させる
		.SetSystemMS("ms/static/Normal.MS.hlsl")
		.Build();
	collider_.backFacePSO.Copy(collider_.frontFacePSO)	// PSOをコピー
		.SetRasterizerState(D3D12_CULL_MODE_FRONT)	// 裏面のみを描画
		.Build();
	collider_.checkResultRoot.AddTableParameter(0, SV_All)
		.AddTableParameter(1, SV_All)
		.AddUAVParameter(0, SV_All)
		.Build();
	collider_.checkResultPSO.Init(collider_.checkResultRoot, PSO::Type::Compute)
		.SetSystemCS("ms/particle/ResultCheck.CS.hlsl")
		.Build();

	// リソース初期化
	collider_.id.InitUAV();
	collider_.depthStencil.InitGPUCollider();
}

void ParticleRenderer::DrawCall(ID3D12GraphicsCommandList6* list) {
	// パーティクルが１つもない場合は処理を行わない
	if (particles_.size() < 0) { return; }

	ID3D12DescriptorHeap* descriptorHeaps[] = { SRV::GetInstance()->GetHeap() };
	list->SetDescriptorHeaps(1, descriptorHeaps);
	for (GPUParticle* p : particles_) {
		// 初期化が必要かチェック
		if (p->GetIsInit()) {
			// 初期化
			list->SetComputeRootSignature(initShader_.root);
			list->SetPipelineState(initShader_.pso.GetState());
			list->SetComputeRootConstantBufferView(0, p->GetCountView());
			list->SetComputeRootDescriptorTable(1, p->GetUAVDataView());
			list->SetComputeRootDescriptorTable(2, p->GetFreeListIndexView());
			list->SetComputeRootDescriptorTable(3, p->GetFreeListView());
			list->Dispatch(p->GetMultiply(), 1, 1);	// 1回実行する

			p->ClearIsInit();	// 初期化フラグをfalseに
		}

		// 必要なバッファを用意
		list->SetComputeRootSignature(*p->GetRoot());
		list->SetComputeRootConstantBufferView(0, FrameTracker::GetInstance()->GetPreFrameBufferView());
		list->SetComputeRootConstantBufferView(1, p->GetEmitterView());
		list->SetComputeRootConstantBufferView(2, p->GetCountView());
		list->SetComputeRootDescriptorTable(3, p->GetUAVDataView());
		list->SetComputeRootDescriptorTable(4, p->GetFreeListIndexView());
		list->SetComputeRootDescriptorTable(5, p->GetFreeListView());
		
		// 生成が必要かチェック
		if (p->GetIsEmit()) {
			int c = p->GetEmitCount();
			// emitが0で実行するとバグるので回避
			if (c != 0) {
				list->SetPipelineState(p->GetEmitterPSO()->GetState());
				list->Dispatch(c, 1, 1);	// 必要分実行する
				p->SetDataBarrier(list);	// 依存関係を設定
			}

			p->ClearIsEmit();	// 初期化フラグをfalseに
		}

		// 更新処理
		list->SetPipelineState(p->GetUpdatePSO()->GetState());
		list->Dispatch(p->GetMultiply(), 1, 1);	// 倍率回実行する

		p->SetDataBarrier(list);	// 依存関係を設定

		// 当たり判定用の処理
		CheckCollision(list, p, target_.back().view);

		// 描画処理（ターゲット分ループする）
		list->SetGraphicsRootSignature(renderingShader_.root);	// 描画用のRootとPSOに変更
		list->SetPipelineState(renderingShader_.pso.GetState());
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
	particles_.clear();
}

void ParticleRenderer::CheckCollision(ID3D12GraphicsCommandList6* list, Object::GPUParticle* p, D3D12_GPU_VIRTUAL_ADDRESS cameraView) {
#pragma region ヒットしたリストの初期化
	// 初期化
	list->SetComputeRootSignature(initHitShader_.root);
	list->SetPipelineState(initHitShader_.pso.GetState());
	list->SetComputeRootConstantBufferView(0, p->GetCountView());
	list->SetComputeRootDescriptorTable(1, p->GetUAVHiTListView());
	list->Dispatch(p->GetMultiply(), 1, 1);	// 1回実行する
#pragma endregion

#pragma region 辺の描画
	list->SetGraphicsRootSignature(renderingShader_.root);	// 描画用のRootとPSOに変更
	list->SetPipelineState(collider_.wireFrame.GetState());
	// 描画先のRTVとDSVを設定する
	list->OMSetRenderTargets(1, &collider_.id.rtvInfo.cpuView, false, &collider_.depthStencil.dsvInfo.cpuView);

	collider_.id.ChangeResourceBarrier(D3D12_RESOURCE_STATE_RENDER_TARGET, list);
	collider_.depthStencil.ChangeResourceBarrier(D3D12_RESOURCE_STATE_DEPTH_WRITE, list);
	collider_.id.Clear(list);
	collider_.depthStencil.Clear(list);

	// ビューポート
	D3D12_VIEWPORT viewport = collider_.depthStencil.GetViewport();
	// viewportを設定
	list->RSSetViewports(1, &viewport);

	// シザー矩形
	D3D12_RECT scissorRect = collider_.depthStencil.GetScissorRect();
	// Scirssorを設定
	list->RSSetScissorRects(1, &scissorRect);

	// GPUパーティクルの辺を描画
	DispatchAllParticle(list, p, cameraView);
#pragma endregion

#pragma region その他オブジェクトの両面ステンシルテスト
	// 描画先のDSVを設定する
	list->OMSetRenderTargets(0, nullptr, false, &collider_.depthStencil.dsvInfo.cpuView);

	list->SetGraphicsRootSignature(collider_.frontFaceRoot);	// Rootセット
	list->SetPipelineState(collider_.frontFacePSO.GetState());	// PSOセット
	DispatchAllModel(list, cameraView);		// 表面を描画
	list->SetPipelineState(collider_.backFacePSO.GetState());	// PSOセット
	DispatchAllModel(list, cameraView);		// 裏面を描画
#pragma endregion
/*
#pragma region ステンシルからヒットしてるIDを検出（できてない）
	//バリアを設定
	collider_.id.ChangeResourceBarrier(D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, list);
	collider_.depthStencil.ChangeResourceBarrier(D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, list);

	list->SetComputeRootSignature(collider_.checkResultRoot);	// Rootセット
	list->SetPipelineState(collider_.checkResultPSO.GetState());	// PSOセット
	list->SetComputeRootDescriptorTable(0, collider_.id.srvInfo.gpuView);
	list->SetComputeRootDescriptorTable(1, collider_.depthStencil.srvInfo.gpuView);
	list->SetComputeRootDescriptorTable(2, p->GetUAVHiTListView());
	list->Dispatch(collider_.depthStencil.width, collider_.depthStencil.height, 1);	// 解像度分実行する
#pragma endregion
#pragma region ヒットしてるパーティクルの処理を呼び出す
	p->SetDataBarrier(list);	// 依存関係を設定
	p->SetHitListBarrier(list);	// 依存関係を設定

	// 必要なバッファを用意
	list->SetComputeRootSignature(*p->GetRoot());
	list->SetPipelineState(p->GetHitPSO()->GetState());
	list->SetComputeRootConstantBufferView(0, FrameTracker::GetInstance()->GetPreFrameBufferView());
	list->SetComputeRootConstantBufferView(1, p->GetEmitterView());
	list->SetComputeRootConstantBufferView(2, p->GetCountView());
	list->SetComputeRootDescriptorTable(3, p->GetUAVDataView());
	list->SetComputeRootDescriptorTable(4, p->GetFreeListIndexView());
	list->SetComputeRootDescriptorTable(5, p->GetFreeListView());
	list->SetComputeRootDescriptorTable(6, p->GetSRVHiTListView());

	list->Dispatch(p->GetMultiply(), 1, 1);	// 倍率回実行する
#pragma endregion
*/
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

	// メッシュレットのプリミティブ数分メッシュシェーダーを実行
	list->DispatchMesh(data->GetMeshletCount(), 1, 1);
}

void ParticleRenderer::DispatchAllModel(ID3D12GraphicsCommandList6* list, D3D12_GPU_VIRTUAL_ADDRESS cameraView) {
	// 全モデル分ループ
	auto models = Resource::Manager::GetInstance()->GetModels();
	SRV* srv = SRV::GetInstance();

	// -------------------------------------------------------------- //

	// StaticModelをDispatch
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
}
