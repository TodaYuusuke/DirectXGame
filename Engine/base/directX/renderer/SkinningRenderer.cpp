#include "SkinningRenderer.h"

#include "base/ImGuiManager.h"
#include "Config.h"

using namespace LWP::Base;

// サイズをここで指定
SkinningRenderer::SkinningRenderer() :
	//vertexBuffer(lwpC::Rendering::kMaxVertex),
	indexBuffer(lwpC::Rendering::kMaxIndex),
	mappedPalette(lwpC::Rendering::kMaxIndex) {}

void SkinningRenderer::Init(GPUDevice* device, SRV* srv, DXC* dxc, std::function<void()> func) {
	// StructuredBufferを初期化
	//vertexBuffer.Init(device, srv);
	indexBuffer.Init(device, srv);
	mappedPalette.Init(device, srv);
	// RootSignatureを生成
	root.AddTableParameter(0, SV_All)	// インデックスのデータ
		.AddCBVParameter(0, SV_All)	// 描画に使うViewprojection
		.AddCBVParameter(1, SV_All)	// 全画面で共通のデータ
		.AddTableParameter(1, SV_Vertex)	// 頂点データ
		.AddTableParameter(2, SV_Vertex)	// MatrixPalette
		.AddTableParameter(3, SV_Vertex)	// トランスフォーム
		.AddTableParameter(1, SV_Pixel)	// マテリアル
		.AddTableParameter(2, SV_Pixel)	// 平行光源
		.AddTableParameter(3, SV_Pixel)	// 点光源
		.AddTableParameter(0, SV_Pixel, 1, lwpC::Rendering::kMaxTexture)	// テクスチャ
		.AddTableParameter(0, SV_Pixel, 2, lwpC::Shadow::Direction::kMaxCount)	// 平行光源のシャドウマップ
		.AddTableParameter(0, SV_Pixel, 3, lwpC::Shadow::Point::kMaxCount)	// 点光源のシャドウマップ
		.AddSampler(0, SV_Pixel)	// テクスチャ用サンプラー
		.AddSampler(1, SV_Pixel, D3D12_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT, D3D12_COMPARISON_FUNC_LESS_EQUAL)	// 平行光源のシャドウマップ用サンプラー
		.AddSampler(2, SV_Pixel, D3D12_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT, D3D12_COMPARISON_FUNC_LESS_EQUAL		// 点光源のシャドウマップ用サンプラー
			, D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP)
		.Build(device->GetDevice());
	// PSOを生成
	pso.Init(root, dxc)
		.SetVertexShader("SkinningObject3d.VS.hlsl")
		.SetPixelShader("Object3d.PS.hlsl")
		.Build(device->GetDevice());

	// 関数セット
	setViewFunction_ = func;
}

void SkinningRenderer::DrawCall(ID3D12GraphicsCommandList* list) {

	// RootSignatureを設定
	list->SetGraphicsRootSignature(root);
	// Viewを先にセット
	setViewFunction_();
	// インフルエンスたちのViewもセット
	//list->SetGraphicsRootDescriptorTable(3, vertexBuffer.GetGPUView());
	//list->SetGraphicsRootDescriptorTable(4, mappedInfluence.GetGPUView());
	list->SetGraphicsRootDescriptorTable(4, mappedPalette.GetGPUView());

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

		// Viewをセット
		list->SetGraphicsRootConstantBufferView(1, it->view);

		// PSOを設定
		list->SetPipelineState(pso.GetState());
		// ディスクリプタテーブルを登録
		list->SetGraphicsRootDescriptorTable(0, indexBuffer.GetGPUView());
		// 全三角形を１つのDrawCallで描画
		list->DrawInstanced(3, indexBuffer.GetCount() / 3, 0, 0);

		// バリアを元に戻す
		it->back->ChangeResourceBarrier(beforeBarrier, list);
	}
}

void SkinningRenderer::Reset() {
	target_.clear();

	//vertexBuffer.Reset();
	indexBuffer.Reset();
	//mappedInfluence.Reset();
	mappedPalette.Reset();
}

