#include "OutLine.h"

#include "component/System.h"

using namespace LWP;
using namespace LWP::Base::PostProcess;

void OutLine::Init() {
	//intensity = 1.0f;
	buffer_.Init(System::engine->directXCommon_->GetGPUDevice());
}
void OutLine::Update() {
	//*buffer_.data_ = intensity;
}

void OutLine::WriteBinding(std::ofstream* stream, RootSignature* root, int* i) {
	// Bindする番号を保持
	bindIndex = *i;
	std::string str = R"(
struct OutLineData {
	float32_t4x4 projectionInverse;
};
ConstantBuffer<OutLineData> olData : register(b${v});

static const float32_t2 kIndex3x3[3][3] = {
	{{-1.0f,-1.0f},{ 0.0f,-1.0f},{ 1.0f,-1.0f}},
	{{-1.0f, 0.0f},{ 0.0f, 0.0f},{ 1.0f, 0.0f}},
	{{-1.0f, 1.0f},{ 0.0f, 1.0f},{ 1.0f, 1.0f}},
};
static const float32_t kPrewittHorizontalKernel[3][3] = {
	{ -1.0f / 6.0f, 0.0f, 1.0f / 6.0f },
	{ -1.0f / 6.0f, 0.0f, 1.0f / 6.0f },
	{ -1.0f / 6.0f, 0.0f, 1.0f / 6.0f },
};
static const float32_t kPrewittVerticalKernel[3][3] = {
	{ -1.0f / 6.0f, -1.0f / 6.0f, -1.0f / 6.0f },
	{ 0.0f, 0.0f, 0.0f },
	{ 1.0f / 6.0f, 1.0f / 6.0f, 1.0f / 6.0f },
};

float32_t OutLine(float32_t2 uv) {
	float32_t2 difference = float32_t2(0.0f, 0.0f);
	uint32_t width, height;
	gTexture.GetDimensions(width, height);
	float32_t2 uvStepSize = float32_t2(rcp(width), rcp(height));

	for(int32_t x = 0; x < 3; x++) {
		for(int32_t y = 0; y < 3; y++) {
			float32_t2 texcoord = uv + kIndex3x3[x][y] * uvStepSize;
			float32_t ndcDepth = gDepth.Sample(gPointSampler, texcoord);

			float32_t4 viewSpace = mul(float32_t4(0.0f,0.0f,ndcDepth,1.0f), olData.projectionInverse);
			float32_t viewZ = viewSpace.z * rcp(viewSpace.w);
			difference.x += viewZ * kPrewittHorizontalKernel[x][y];
			difference.y += viewZ * kPrewittVerticalKernel[x][y];
		}
	}

	float32_t weight = length(difference);
	return (1.0f - weight);
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
void OutLine::WriteProcess(std::ofstream* stream) {
	*stream << R"(
	output.rgb *= OutLine(uv);
)";
}
		
void OutLine::BindCommand(ID3D12GraphicsCommandList* list, int* offset) {
	list->SetGraphicsRootConstantBufferView(bindIndex + *offset, buffer_.GetGPUView());
}

void OutLine::DebugGUI() {
	if (ImGui::TreeNode("OutLine")) {
		ImGui::Checkbox("Use", &use);
		ImGui::TreePop();
	}
}