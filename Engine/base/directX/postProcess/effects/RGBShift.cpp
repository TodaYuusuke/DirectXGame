#include "RGBShift.h"

#include "component/System.h"

using namespace LWP;
using namespace LWP::Base::PostProcess;

void RGBShift::Init() {
	name = "RGBShift";
	shiftR = { 0.005f, 0.0f };
	shiftG = { 0.0f, 0.0f };
	shiftB = { -0.005f, 0.0f };
	buffer_.Init();
}
void RGBShift::Update() {
	buffer_.data_->shiftR = shiftR;
	buffer_.data_->shiftG = shiftG;
	buffer_.data_->shiftB = shiftB;
}

void RGBShift::WriteBinding(std::ofstream* stream, RootSignature* root, int* i) {
	// Bindする番号を保持
	bindIndex = *i;
	std::string str = R"(
struct RGBShiftData {
	float32_t2 shiftR;
	float32_t2 shiftG;
	float32_t2 shiftB;
};
ConstantBuffer<RGBShiftData> rgbShiftData : register(b${v});

float32_t3 RGBShift(float32_t2 uv) {
    float32_t r = gTexture.Sample(gSampler, uv + rgbShiftData.shiftR).r;
    float32_t g = gTexture.Sample(gSampler, uv + rgbShiftData.shiftG).g;
    float32_t b = gTexture.Sample(gSampler, uv + rgbShiftData.shiftB).b;
    return float32_t3(r, g, b);
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
void RGBShift::WriteProcess(std::ofstream* stream) {
	*stream << R"(
	output.rgb = RGBShift(uv);
)";
}

void RGBShift::BindCommand(ID3D12GraphicsCommandList* list, int* offset) {
	list->SetGraphicsRootConstantBufferView(bindIndex + *offset, buffer_.GetGPUView());
}

void RGBShift::DebugGUI() {
	ImGui::DragFloat2("Shift R", &shiftR.x, 0.001f);
	ImGui::DragFloat2("Shift G", &shiftG.x, 0.001f);
	ImGui::DragFloat2("Shift B", &shiftB.x, 0.001f);
	ImGui::Checkbox("Use", &use);
}
