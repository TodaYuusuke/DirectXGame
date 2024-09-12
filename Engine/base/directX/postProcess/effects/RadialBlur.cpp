#include "RadialBlur.h"

#include "component/System.h"

using namespace LWP;
using namespace LWP::Base;
using namespace LWP::Base::PostProcess;

void RadialBlur::Init() {
	center = { 0.5f,0.5f };
	blurWidth = 0.01f;
	buffer_.Init(System::engine->directXCommon_->GetGPUDevice());
}
void RadialBlur::Update() {
	buffer_.data_->center = center;
	buffer_.data_->blurWidth = blurWidth;
}

void RadialBlur::WriteBinding(std::ofstream* stream, RootSignature* root, int* i) {
	// Bindする番号を保持
	bindIndex = *i;
	std::string str = R"(
struct RadialBlurData {
	float32_t2 center;
	float32_t blurWidth;
};
ConstantBuffer<RadialBlurData> radialData : register(b${v});

float32_t3 RadialBlur(float32_t3 rgb, float32_t2 uv) {
	const int32_t kNumSamples = 10;
	float32_t3 result = rgb;

	float32_t2 direction = uv - radialData.center;
	for(int32_t sampleIndex = 1; sampleIndex < kNumSamples; sampleIndex++) {
		float32_t2 texcoord = uv + direction * radialData.blurWidth * float32_t(sampleIndex);
		result += gTexture.Sample(gSampler, texcoord).rgb;
	}

	return result * rcp(kNumSamples);
})";
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
void RadialBlur::WriteProcess(std::ofstream* stream) {
	*stream << R"(
	output.rgb = RadialBlur(output.rgb, uv);
)";
}
		
void RadialBlur::BindCommand(ID3D12GraphicsCommandList* list, int* offset) {
	list->SetGraphicsRootConstantBufferView(bindIndex + *offset, buffer_.GetGPUView());
}

void RadialBlur::DebugGUI() {
	if (ImGui::TreeNode("RadialBlur")) {
		ImGui::DragFloat2("Center", &center.x, 0.01f);
		ImGui::DragFloat("BlurWidth", &blurWidth, 0.001f);
		ImGui::Checkbox("Use", &use);
		ImGui::TreePop();
	}
}