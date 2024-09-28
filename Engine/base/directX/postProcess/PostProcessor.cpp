#include "PostProcessor.h"

#include "base/directX/utility/HeapManager.h"
#include "component/System.h"
#include <vector>

using namespace LWP;
using namespace LWP::Base;
using namespace LWP::Base::PostProcess;

void PostProcessor::Init() {
	bloom.Init();
	outLine.Init();
	radialBlur.Init();
	grayScale.Init();
	vignetting.Init();
	CreateShaderFile();
}
void PostProcessor::Update() {
	bloom.Update();
	outLine.Update();
	radialBlur.Update();
	grayScale.Update();
	vignetting.Update();
}

void PostProcessor::CreateShaderFile() {
	// １つも使用しない場合は帰る
	//if (!UseFlag()) { return; }

	// GPUデバイスのポインタ
	GPUDevice* dev = System::engine->directXCommon_->GetGPUDevice();
	// HeapManagerのポインタ
	DXC* dxc = System::engine->directXCommon_->GetDXC();

	// 共通化してまとめて処理
	std::vector<IPostProcess*> vec = GetAllProcess();
	processes_.clear();

	// ファイルストリームを作成
	std::ofstream shader;
	shader.open("resources/system/shaders/postProcess/temp.PS.hlsl", std::ios::trunc);	// ファイルをオープン

	// 最初に必要な定義を行う
	shader << R"(
struct PSInput
{
    float32_t4 position : SV_POSITION;
    float32_t2 texcoord : TEXCOORD0;
};

struct Parameter
{
    int time;
    int rWidth;
    int rHeight;
};

ConstantBuffer<Parameter> gPara : register(b0);
Texture2D<float32_t4> gTexture : register(t0);
Texture2D<float32_t> gDepth : register(t1);
SamplerState gSampler : register(s0);
SamplerState gPointSampler : register(s1);
)";
	// 初期化
	root_.Init();
	// 既定値設定
	root_ = root_.AddCBVParameter(0, SV_Pixel)	// レンダリング用のデータ
		.AddTableParameter(0, SV_Pixel)	// レンダリングに使うテクスチャ
		.AddTableParameter(1, SV_Pixel)	// レンダリングに使う深度マップ
		.AddSampler(0, SV_Pixel)		// テクスチャのサンプラー
		.AddSampler(1, SV_Pixel, D3D12_FILTER_MIN_MAG_MIP_POINT);	// デプステクスチャのサンプラー
	// 宣言の書き込み & RootSignature生成
	int b = 1;	// Binding用の通し番号
	for (int i = 0; i < vec.size(); i++) {
		// 使用するなら
		if (vec[i]->use) {
			// 書き込み
			vec[i]->WriteBinding(&shader, &root_, &b);
		}
	}
	// 最後にrootSignatureをビルド
	root_.Build(dev->GetDevice());

	shader << R"(
float32_t4 main(PSInput input) : SV_TARGET {
	float32_t4 output;
	float2 uv = input.texcoord;

    output = gTexture.Sample(gSampler, uv);
)";

	// 処理の書き込み
	for (int i = 0; i < vec.size(); i++) {
		// 使用するなら書き込み処理
		if (vec[i]->use) {
			vec[i]->WriteProcess(&shader);
			processes_.push_back(vec[i]);
		}
	}

	shader << R"(
	return output;
}
)";
	
	// シェーダー作成終了
	shader.close();
	// シェーダーファイルを元にPSOを作成
	pso_.Init(root_, dxc)
		.SetDepthStencilState(false)
		.SetVertexShader("postProcess/PassThrough.VS.hlsl")
		.SetPixelShader("postProcess/temp.PS.hlsl")
		.Build(dev->GetDevice());
}

void PostProcessor::CreatePSO(std::string filePath) {
	processes_.clear();
	// ほんとはよくない設計なので代替案募集

	// GPUデバイスのポインタ
	GPUDevice* dev = System::engine->directXCommon_->GetGPUDevice();
	// HeapManagerのポインタ
	DXC* dxc = System::engine->directXCommon_->GetDXC();


	// RootSignature生成
	root_.AddCBVParameter(0, SV_Pixel)	// レンダリング用のデータ
		.AddTableParameter(0, SV_Pixel)	// レンダリングに使うテクスチャ
		.AddTableParameter(1, SV_Pixel)	// レンダリングに使う深度マップ
		.AddSampler(0, SV_Pixel)	// テクスチャのサンプラー
		.AddSampler(1, SV_Pixel, D3D12_FILTER_MIN_MAG_MIP_POINT)	// デプステクスチャのサンプラー
	.Build(dev->GetDevice());
	// PSO生成
	pso_.Init(root_, dxc)
		.SetDepthStencilState(false)
		.SetVertexShader("postProcess/PassThrough.VS.hlsl")
		.SetPixelShader(filePath)
		.Build(dev->GetDevice());
}

void PostProcessor::PreCommands(ID3D12GraphicsCommandList* list, Base::RenderResource* target) {
	for (int i = 0; i < processes_.size(); i++) {
		// 書き込み処理
		processes_[i]->PreCommand(list, target);
	}
}

void PostProcessor::SetCommands(ID3D12GraphicsCommandList* list) {
	// RootSignatureとPSOをセット
	list->SetGraphicsRootSignature(root_);
	list->SetPipelineState(pso_.GetState());
	// 共通化してまとめて処理
	int offset = 2;
	for (int i = 0; i < processes_.size(); i++) {
		// 書き込み処理
		processes_[i]->BindCommand(list, &offset);
	}
}

void PostProcessor::DebugGUI() {
	if (ImGui::TreeNode("PostProcess")) {
		bloom.DebugGUI();
		outLine.DebugGUI();
		radialBlur.DebugGUI();
		grayScale.DebugGUI();
		vignetting.DebugGUI();
		ImGui::Text("----------");
		ImGui::Checkbox("Use", &use);
		if (use && ImGui::Button("Update Shader")) { CreateShaderFile(); }
		ImGui::TreePop();
	}
}

std::vector<IPostProcess*> PostProcessor::GetAllProcess() {
	// 共通化してまとめて処理
	std::vector<IPostProcess*> result;
	result.push_back(&bloom);
	result.push_back(&outLine);
	result.push_back(&radialBlur);
	result.push_back(&grayScale);
	result.push_back(&vignetting);

	return result;
}