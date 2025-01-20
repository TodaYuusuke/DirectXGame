#include "GrayScale.h"

#include "component/System.h"

using namespace LWP;
using namespace LWP::Base::PostProcess;

void GrayScale::Init() {
	intensity = 1.0f;
	buffer_.Init();
}
void GrayScale::Update() {
	*buffer_.data_ = intensity;
}

void GrayScale::WriteBinding(std::ofstream* stream, RootSignature* root, int* i) {
	// Bindする番号を保持
	bindIndex = *i;
	std::string str = R"(
struct GrayScaleData {
	float32_t intensity;
};
ConstantBuffer<GrayScaleData> gsData : register(b${v});

float32_t3 GrayScale(float32_t3 color) {
    float32_t3 result = dot(color, float32_t3(0.2125f, 0.7154f, 0.0721f));
    return lerp(color, result, gsData.intensity);
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
void GrayScale::WriteProcess(std::ofstream* stream) {
	*stream << R"(
	output.rgb = GrayScale(output.rgb);
)";
}
		
void GrayScale::BindCommand(ID3D12GraphicsCommandList* list, int* offset) {
	list->SetGraphicsRootConstantBufferView(bindIndex + *offset, buffer_.GetGPUView());
}

void GrayScale::DebugGUI() {
	if (ImGui::TreeNode("GrayScale")) {
		ImGui::DragFloat("Intensity", &intensity, 0.01f);
		ImGui::Checkbox("Use", &use);
		ImGui::TreePop();
	}
}