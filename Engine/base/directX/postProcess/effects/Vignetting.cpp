#include "Vignetting.h"

#include "component/System.h"

using namespace LWP;
using namespace LWP::Base::PostProcess;

void Vignetting::Init() {
	intensity = 1.0f;
	buffer_.Init(System::engine->directXCommon_->GetGPUDevice());
}
void Vignetting::Update() {
	*buffer_.data_ = intensity;
}

void Vignetting::WriteBinding(std::ofstream* stream, RootSignature* root, int* i) {
	// Bindする番号を保持
	bindIndex = *i;
	std::string str = R"(
struct VignettingData {
	float32_t intensity;
};
ConstantBuffer<VignettingData> vData : register(b${v});

float32_t Vignetting(float32_t2 uv) {
	float32_t2 correct = uv * (1.0f - uv.xy);
	float vignette = correct.x * correct.y * 16.0f;
	vignette = saturate(pow(vignette, vData.intensity));
	return vignette;
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
void Vignetting::WriteProcess(std::ofstream* stream) {
	*stream << R"(
	output.rgb *= Vignetting(uv);
)";
}
		
void Vignetting::BindCommand(ID3D12GraphicsCommandList* list, int* offset) {
	list->SetGraphicsRootConstantBufferView(bindIndex + *offset, buffer_.GetGPUView());
}

void Vignetting::DebugGUI() {
	if (ImGui::TreeNode("Vignetting")) {
		ImGui::DragFloat("Intensity", &intensity, 0.01f);
		ImGui::Checkbox("Use", &use);
		ImGui::TreePop();
	}
}