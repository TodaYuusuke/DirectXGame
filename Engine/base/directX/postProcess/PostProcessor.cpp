#include "PostProcessor.h"

#include "base/directX/utility/HeapManager.h"
#include "component/System.h"
#include <vector>

using namespace LWP;
using namespace LWP::Base;
using namespace LWP::Base::PostProcess;
/*
void PostProcessor::CreateShaderFile() {
	// 共通化してまとめて処理
	std::vector<IPostProcess*> vec;
	vec.push_back(&grayScale);

	// ファイルストリームを作成
	std::ofstream shader;
	shader.open("resources/system/shaders/postProcess/temp.PS.hlsl", std::ios_base::app);	// ファイルをオープン
	shader << R"(#include "PassThrough.hlsli")";

	// includeの書き込み & RootSignature生成
	for (int i = 0; i < vec.size(); i++) {
		// 使用するなら
		if (vec[i]->use) {
			// 書き込み
			vec[i]->WriteInclude(&shader);
			// RootSignature生成
			root_.AddCBVParameter()
		}
	}

	shader << R"(
float32_t4 main(PSInput input) : SV_TARGET {
	float32_t4 output;
	float2 uv = input.texcoord;

	output = gTexture[1].Sample(gSampler, uv);
)";

	// 処理の書き込み
	for (int i = 0; i < vec.size(); i++) {
		// 使用するなら書き込み処理
		if (vec[i]->use) {
			vec[i]->WriteFunction(&shader);
		}
	}

	shader << R"(
	return output;
}
)";
	// シェーダーファイルを元にPSOを作成
	pso.
}
*/

// 既存のシェーダーファイルからPSOを生成する関数
void PostProcessor::CreatePSO(std::string filePath) {
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
		.Build(dev->GetDevice());
	// PSO生成
	pso_.Init(root_, dxc)
		.SetDepthStencilState(false)
		.SetVertexShader("postProcess/PassThrough.VS.hlsl")
		.SetPixelShader(filePath)
		.Build(dev->GetDevice());
}
