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

	for (int i = 0; i < 2; i++) {
		resource[i].rr.Init(dev, heaps);

		resource[i].root.AddTableParameter(0, SV_Pixel)	// レンダリングに使うテクスチャ
			.AddSampler(0, SV_Pixel)	// テクスチャ用サンプラー
			.Build(dev->GetDevice());

		resource[i].pso.Init(resource[i].root, dxc)
			.SetDepthStencilState(false)
			.SetVertexShader("postProcess/PassThrough.VS.hlsl")
			.SetPixelShader("postProcess/BloomGlayScale.PS.hlsl")
			.Build(dev->GetDevice());
	}
}
void Bloom::Update() {
	*buffer_.data_ = intensity;
}

void Bloom::WriteBinding(std::ofstream* stream, RootSignature* root, int* i) {
	// Bindする番号を保持
	bindIndex = *i;
	std::string str = R"(
struct BloomData {
	float32_t intensity;
};
ConstantBuffer<BloomData> blmData : register(b${v});

float32_t3 Bloom(float32_t3 color) {

}
)";
	// 変数で値を書き換え
	size_t pos;
	while ((pos = str.find("${v}")) != std::string::npos) {
		str.replace(pos, 4, std::to_string(bindIndex));
	}
	*stream << str;	// 書き込み
	// rootSignatureを宣言
	*root = root->AddCBVParameter(bindIndex, SV_Pixel);
	// iを加算
	*i += 1;
}
		// シェーダー内の処理を書き込む
void Bloom::WriteProcess(std::ofstream* stream) {
	*stream << R"(
	output.rgb = GrayScale(output.rgb);
)";
}
		
void Bloom::BindCommand(ID3D12GraphicsCommandList* list, int* offset) {
	list->SetGraphicsRootConstantBufferView(bindIndex + *offset, buffer_.GetGPUView());
	*offset += 1;
	list->SetGraphicsRootConstantBufferView(bindIndex + *offset, buffer_.GetGPUView());
}
void Bloom::PreCommand(ID3D12GraphicsCommandList* list, RenderResource* target) {
	// 処理1個目
	list->OMSetRenderTargets(1, &resource[0].rr.rtvInfo.cpuView, false, nullptr);	// 書き込み先のリソースを指定
	// テクスチャのバインド
	//list->SetGraphicsRootConstantBufferView(0, );
	list->SetGraphicsRootDescriptorTable(1, target->srvInfo.gpuView);
	//list->SetGraphicsRootDescriptorTable(2, it->depth->srvInfo.gpuView);

	// リソースバリアをセット
	target->ChangeResourceBarrier(D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, list);	// 加工する画像
	resource[0].rr.ChangeResourceBarrier(D3D12_RESOURCE_STATE_RENDER_TARGET, list);	// 書き込み対象
	// 全画面クリア
	resource[0].rr.Clear(list);

	// ビューポート
	D3D12_VIEWPORT viewport = {};
	// クライアント領域のサイズと一緒にして画面全体に表示
	viewport.Width = static_cast<float>(resource[0].rr.width);
	viewport.Height = static_cast<float>(resource[0].rr.height);
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
	scissorRect.right = resource[0].rr.width;
	scissorRect.top = 0;
	scissorRect.bottom = resource[0].rr.height;
	// Scirssorを設定
	list->RSSetScissorRects(1, &scissorRect);

	// 描画!
	list->DrawInstanced(3, 1, 0, 0);

	// バリアを元に戻す
	resource[0].rr.ChangeResourceBarrier(D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, list);	// 書き込み対象
}

void Bloom::DebugGUI() {
	if (ImGui::TreeNode("Bloom")) {
		ImGui::DragFloat("Intensity", &intensity, 0.01f);
		ImGui::Checkbox("Use", &use);
		ImGui::TreePop();
	}
}