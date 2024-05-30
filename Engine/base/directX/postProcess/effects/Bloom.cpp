#include "Bloom.h"

#include "component/System.h"

using namespace LWP;
using namespace LWP::Base;
using namespace LWP::Base::PostProcess;

void Bloom::Init() {
	GPUDevice* dev = System::engine->directXCommon_->GetGPUDevice();
	HeapManager* heaps = System::engine->directXCommon_->GetHeaps();
	DXC* dxc = System::engine->directXCommon_->GetDXC();

	intensity = 1.0f;


	buffer_.Init(dev);


	resource[0].rr.Init(dev, heaps);
	resource[1].rr.Init(dev, heaps);

	resource[0].root.AddTableParameter(0, SV_Pixel)	// レンダリングに使うテクスチャ
		.AddSampler(0, SV_Pixel)	// テクスチャ用サンプラー
		.Build(dev->GetDevice());
	resource[0].pso.Init(resource[0].root, dxc)
		.SetDepthStencilState(false)
		.SetVertexShader("postProcess/PassThrough.VS.hlsl")
		.SetPixelShader("postProcess/BloomGlayScale.PS.hlsl")
		.Build(dev->GetDevice());

	resource[1].root.AddTableParameter(0, SV_Pixel)	// レンダリングに使うテクスチャ
		.AddSampler(0, SV_Pixel)	// テクスチャ用サンプラー
		.Build(dev->GetDevice());
	resource[1].pso.Init(resource[1].root, dxc)
		.SetDepthStencilState(false)
		.SetBlendState(PSO::BlendMode::Add)
		.SetVertexShader("postProcess/PassThrough.VS.hlsl")
		.SetPixelShader("postProcess/BloomGaussianBlur.PS.hlsl")
		.Build(dev->GetDevice());
}
void Bloom::Update() {
	*buffer_.data_ = intensity;
}

void Bloom::WriteBinding(std::ofstream* stream, RootSignature* root, int* i) {
	stream; root; i;
//	// Bindする番号を保持
//	bindIndex = *i;
//	std::string str = R"(
//struct BloomData {
//	float32_t intensity;
//};
//ConstantBuffer<BloomData> blmData : register(b${v});
//
//float32_t3 Bloom(float32_t3 color) {
//
//}
//)";
//	// 変数で値を書き換え
//	size_t pos;
//	while ((pos = str.find("${v}")) != std::string::npos) {
//		str.replace(pos, 4, std::to_string(bindIndex));
//	}
//	*stream << str;	// 書き込み
//	// rootSignatureを宣言
//	*root = root->AddCBVParameter(bindIndex, SV_Pixel);
//	// iを加算
//	*i += 1;
}
		// シェーダー内の処理を書き込む
void Bloom::WriteProcess(std::ofstream* stream) {
	stream;
/*	*stream << R"(
	output.rgb = GrayScale(output.rgb);
)";*/
}
		
void Bloom::BindCommand(ID3D12GraphicsCommandList* list, int* offset) {
	list; offset;
	//list->SetGraphicsRootConstantBufferView(bindIndex + *offset, buffer_.GetGPUView());
	//*offset += 1;
	//list->SetGraphicsRootConstantBufferView(bindIndex + *offset, buffer_.GetGPUView());
}
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
	list->OMSetRenderTargets(1, &resource[0].rr.rtvInfo.cpuView, false, nullptr);	// 書き込み先のリソースを指定
	list->SetGraphicsRootSignature(resource[0].root);
	list->SetPipelineState(resource[0].pso.GetState());
	// テクスチャのバインド
	list->SetGraphicsRootDescriptorTable(0, target->srvInfo.gpuView);

	// リソースバリアをセット
	target->ChangeResourceBarrier(D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, list);	// 加工する画像
	resource[0].rr.ChangeResourceBarrier(D3D12_RESOURCE_STATE_RENDER_TARGET, list);	// 書き込み対象
	// 全画面クリア
	resource[0].rr.Clear(list);

	// viewportを設定
	list->RSSetViewports(1, &viewport);
	// Scirssorを設定
	list->RSSetScissorRects(1, &scissorRect);

	// 描画!
	list->DrawInstanced(3, 1, 0, 0);
	// バリアを元に戻す
	resource[0].rr.ChangeResourceBarrier(D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, list);	// 書き込み対象


// ******** 2．輝度をぼかしてAddブレンド ******** //
	list->OMSetRenderTargets(1, &target->rtvInfo.cpuView, false, nullptr);	// 書き込み先のリソースを指定
	list->SetGraphicsRootSignature(resource[1].root);
	list->SetPipelineState(resource[1].pso.GetState());
	// リソースバリアをセット
	target->ChangeResourceBarrier(D3D12_RESOURCE_STATE_RENDER_TARGET, list);
	//target->Clear(list);
	// テクスチャのバインド
	list->SetGraphicsRootDescriptorTable(0, resource[0].rr.srvInfo.gpuView);
	// viewportを設定
	list->RSSetViewports(1, &viewport);
	// Scirssorを設定
	list->RSSetScissorRects(1, &scissorRect);

	// 描画!
	list->DrawInstanced(3, 1, 0, 0);
}

void Bloom::DebugGUI() {
	if (ImGui::TreeNode("Bloom")) {
		ImGui::DragFloat("Intensity", &intensity, 0.01f);
		ImGui::Checkbox("Use", &use);
		ImGui::TreePop();
	}
}