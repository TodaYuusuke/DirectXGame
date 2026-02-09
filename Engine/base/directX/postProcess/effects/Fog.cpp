#include "Fog.h"
#include "component/System.h"

using namespace LWP;
using namespace LWP::Base;
using namespace LWP::Base::PostProcess;

void Fog::Init() {
	fogNear = 10.0f;
	fogFar = 100.0f;
	color = Utility::ColorPattern::WHITE;
	buffer_.Init();
}

void Fog::Update() {
	buffer_.data_->color = color.GetVector4();
	buffer_.data_->fogNear = fogNear;
	buffer_.data_->fogFar = fogFar;
}

void Fog::WriteBinding(std::ofstream* stream, RootSignature* root, int* i) {
	// Bindする番号を保持
	bindIndex = *i;
	std::string str = R"(
struct FogData {
	float32_t4x4 projectionInverse;
	float32_t4 color;
	float32_t fogNear;
	float32_t fogFar;
};
ConstantBuffer<FogData> fogData : register(b${v});

float32_t3 ApplyFog(float32_t2 uv, float32_t3 color) {
	float32_t ndcDepth = gDepth.Sample(gPointSampler, uv);
	
	// NDCからビュー空間へ変換（Zのみ）
	float32_t4 viewSpace = mul(float32_t4(0.0f, 0.0f, ndcDepth, 1.0f), fogData.projectionInverse);
	float32_t viewZ = viewSpace.z * rcp(viewSpace.w);
	
	// 霧の係数を計算（線形）
	float32_t fogFactor = saturate((viewZ - fogData.fogNear) / (fogData.fogFar - fogData.fogNear));
	fogFactor *= fogData.color.a;	// アルファ値を強度として使用
	
	return lerp(color, fogData.color.rgb, fogFactor);
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

void Fog::WriteProcess(std::ofstream* stream) {
	*stream << R"(
	output.rgb = ApplyFog(uv, output.rgb);
)";
}

void Fog::BindCommand(ID3D12GraphicsCommandList* list, int* offset) {
	list->SetGraphicsRootConstantBufferView(bindIndex + *offset, buffer_.GetGPUView());
}

void Fog::DebugGUI() {
	if (ImGui::TreeNode("Fog")) {
		ImGui::DragFloat("Near", &fogNear, 0.1f);
		ImGui::DragFloat("Far", &fogFar, 0.1f);
		ImGuiManager::ColorEdit4("Color", color);
		ImGui::Checkbox("Use", &use);
		ImGui::TreePop();
	}
}
