#include "Glitch.h"

#include "component/System.h"

using namespace LWP;
using namespace LWP::Base::PostProcess;

void Glitch::Init() {
	name = "Glitch";
	strength = 0.5f;
	buffer_.Init();
}
void Glitch::Update() {
	*buffer_.data_ = strength;
}

void Glitch::WriteBinding(std::ofstream* stream, RootSignature* root, int* i) {
	// Bindする番号を保持
	bindIndex = *i;
	std::string str = R"(
struct GlitchData {
	float32_t strength;
};
ConstantBuffer<GlitchData> glitchData : register(b${v});


float32_t3 Glitch(float32_t2 uv, float32_t time) {
    float32_t2 pixelUV = uv;
    
    // ブロック分解能の計算（時間で変化させる）
    float32_t blockTime = floor(time * 10.0);
    RandomGenerator resRand;
    resRand.seed = float32_t3(blockTime, 0, 0);
    float32_t2 resScale = 0.5 + resRand.Generate2d() * 2.0;
    float32_t2 blockRes = float32_t2(10.0, 30.0) * resScale;

    // ブロックノイズの計算
    float32_t2 blockPos = floor(uv * blockRes);
    RandomGenerator rand;
    rand.seed = float32_t3(blockTime, blockPos.x, blockPos.y);
    float32_t noise = rand.Generate1d();
    
    if (noise < glitchData.strength * 0.2) {
        // 横方向へのズレ
        RandomGenerator shiftRand;
        shiftRand.seed = float32_t3(blockPos.y, blockTime, 0);
        float32_t shift = (shiftRand.Generate1d() - 0.5) * 0.1 * glitchData.strength;
        pixelUV.x += shift;
    }
    
    // カラーチャンネルの分離も少し混ぜる
    float32_t3 color;
    color.r = gTexture.Sample(gSampler, pixelUV + float32_t2(0.002, 0.0) * glitchData.strength).r;
    color.g = gTexture.Sample(gSampler, pixelUV).g;
    color.b = gTexture.Sample(gSampler, pixelUV - float32_t2(0.002, 0.0) * glitchData.strength).b;
    
    // たまにブロック状に色を反転させたりする
    //if (noise < glitchData.strength * 0.05) {
    //    color = 1.0 - color;
    //}
    
    return color;
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

void Glitch::WriteProcess(std::ofstream* stream) {
	*stream << R"(
	output.rgb = Glitch(uv, gPara.time);
)";
}

void Glitch::BindCommand(ID3D12GraphicsCommandList* list, int* offset) {
	list->SetGraphicsRootConstantBufferView(bindIndex + *offset, buffer_.GetGPUView());
}

void Glitch::DebugGUI() {
	ImGui::DragFloat("Strength", &strength, 0.01f, 0.0f, 1.0f);
	ImGui::Checkbox("Use", &use);
}
