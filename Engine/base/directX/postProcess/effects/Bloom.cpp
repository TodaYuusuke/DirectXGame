#include "Bloom.h"

#include "component/System.h"

using namespace LWP;
using namespace LWP::Base;
using namespace LWP::Base::PostProcess;

void Bloom::Init() {
	GPUDevice* dev = System::engine->directXCommon_->GetGPUDevice();
	HeapManager* heaps = System::engine->directXCommon_->GetHeaps();
	DXC* dxc = System::engine->directXCommon_->GetDXC();

	threshold = 0.95f;


	buffer_.Init(dev);

	brightnessFilter.rr.Init(dev, heaps);
	gaussX.rr.Init(dev, heaps);
	// 輝度抽出
	brightnessFilter.root.AddCBVParameter(0, SV_Pixel)
		.AddTableParameter(0, SV_Pixel)	// レンダリングに使うテクスチャ
		.AddSampler(0, SV_Pixel)	// テクスチャ用サンプラー
		.Build(dev->GetDevice());
	brightnessFilter.pso.Init(brightnessFilter.root, dxc)
		.SetDepthStencilState(false)
		.SetVertexShader("postProcess/PassThrough.VS.hlsl")
		.SetPixelShader("postProcess/bloom/BrightnessFilter.PS.hlsl")
		.Build(dev->GetDevice());
	// X軸ガウシアンブラー
	gaussX.root.AddTableParameter(0, SV_Pixel)	// レンダリングに使うテクスチャ
		.AddSampler(0, SV_Pixel)	// テクスチャ用サンプラー
		.Build(dev->GetDevice());
	gaussX.pso.Init(gaussX.root, dxc)
		.SetDepthStencilState(false)
		.SetVertexShader("postProcess/PassThrough.VS.hlsl")
		.SetPixelShader("postProcess/bloom/GaussianBlurX.PS.hlsl")
		.Build(dev->GetDevice());
	// Y軸ガウシアンブラー
	gaussY.Init(gaussX.root, dxc)
		.SetDepthStencilState(false)
		.SetBlendState(PSO::BlendMode::Add)
		.SetVertexShader("postProcess/PassThrough.VS.hlsl")
		.SetPixelShader("postProcess/bloom/GaussianBlurY.PS.hlsl")
		.Build(dev->GetDevice());
}
void Bloom::Update() {
	*buffer_.data_ = threshold;
}

void Bloom::WriteBinding(std::ofstream* stream, RootSignature* root, int* i) { stream; root; i; }
void Bloom::WriteProcess(std::ofstream* stream) { stream; }
void Bloom::BindCommand(ID3D12GraphicsCommandList* list, int* offset) { list; offset; }

void Bloom::PreCommand(ID3D12GraphicsCommandList* list, RenderResource* target) {
	// ビューポート
	D3D12_VIEWPORT viewport = {};
	// クライアント領域のサイズと一緒にして画面全体に表示
	viewport.Width = static_cast<float>(target->width);
	viewport.Height = static_cast<float>(target->height);
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	// シザー矩形
	D3D12_RECT scissorRect = {};
	// 基本的にビューポートと同じ矩形が構成されるようにする
	scissorRect.left = 0;
	scissorRect.right = target->width;
	scissorRect.top = 0;
	scissorRect.bottom = target->height;


// ******** 1．輝度を抽出 ******** //
	list->OMSetRenderTargets(1, &brightnessFilter.rr.rtvInfo.cpuView, false, nullptr);	// 書き込み先のリソースを指定
	list->SetGraphicsRootSignature(brightnessFilter.root);
	list->SetPipelineState(brightnessFilter.pso.GetState());
	// テクスチャのバインド
	list->SetGraphicsRootConstantBufferView(0, buffer_.GetGPUView());
	list->SetGraphicsRootDescriptorTable(1, target->srvInfo.gpuView);

	// リソースバリアをセット
	target->ChangeResourceBarrier(D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, list);	// 加工する画像
	brightnessFilter.rr.ChangeResourceBarrier(D3D12_RESOURCE_STATE_RENDER_TARGET, list);	// 書き込み対象
	// 全画面クリア
	brightnessFilter.rr.Clear(list);

	// viewportを設定
	list->RSSetViewports(1, &viewport);
	// Scirssorを設定
	list->RSSetScissorRects(1, &scissorRect);

	// 描画!
	list->DrawInstanced(3, 1, 0, 0);
	// バリアを元に戻す
	brightnessFilter.rr.ChangeResourceBarrier(D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, list);	// 書き込み対象


// ******** 2．X軸の輝度をガウシアンブラー ******** //
	list->OMSetRenderTargets(1, &gaussX.rr.rtvInfo.cpuView, false, nullptr);	// 書き込み先のリソースを指定
	list->SetGraphicsRootSignature(gaussX.root);
	list->SetPipelineState(gaussX.pso.GetState());
	// リソースバリアをセット
	gaussX.rr.ChangeResourceBarrier(D3D12_RESOURCE_STATE_RENDER_TARGET, list);
	gaussX.rr.Clear(list);
	// テクスチャのバインド
	list->SetGraphicsRootDescriptorTable(0, brightnessFilter.rr.srvInfo.gpuView);
	// viewportを設定
	list->RSSetViewports(1, &viewport);
	// Scirssorを設定
	list->RSSetScissorRects(1, &scissorRect);

	// 描画!
	list->DrawInstanced(3, 1, 0, 0);
	// バリアを元に戻す
	gaussX.rr.ChangeResourceBarrier(D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, list);	// 書き込み対象


// ******** 3．Y軸の輝度をガウシアンブラーし、Addブレンド ******** //
	list->OMSetRenderTargets(1, &target->rtvInfo.cpuView, false, nullptr);	// 書き込み先のリソースを指定
	list->SetGraphicsRootSignature(gaussX.root);
	list->SetPipelineState(gaussY.GetState());
	// リソースバリアをセット
	target->ChangeResourceBarrier(D3D12_RESOURCE_STATE_RENDER_TARGET, list);
	// テクスチャのバインド
	list->SetGraphicsRootDescriptorTable(0, gaussX.rr.srvInfo.gpuView);
	// viewportを設定
	list->RSSetViewports(1, &viewport);
	// Scirssorを設定
	list->RSSetScissorRects(1, &scissorRect);

	// 描画!
	list->DrawInstanced(3, 1, 0, 0);
}

void Bloom::DebugGUI() {
	if (ImGui::TreeNode("Bloom")) {
		ImGui::DragFloat("Threshold", &threshold, 0.01f);
		ImGui::Checkbox("Use", &use);
		ImGui::TreePop();
	}
}