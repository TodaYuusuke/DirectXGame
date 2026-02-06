#include "PostProcessor.h"

#include "component/System.h"

// 全てのエフェクトをインクルード（AddPassのため）
#include "effects/Bloom.h"
#include "effects/OutLine.h"
#include "effects/GrayScale.h"
#include "effects/Vignetting.h"
#include "effects/RadialBlur.h"
#include "effects/RGBShift.h"
#include "effects/Glitch.h"

#include <vector>
#include <algorithm>

using namespace LWP;
using namespace LWP::Base;
using namespace LWP::Base::PostProcess;

void PostProcessor::Init() {
	commonBuffer_.Init();
}
void PostProcessor::Update() {
	for (auto& pass : passes_) {
		pass->Update();
	}

	// 共通パラメータの更新
	commonBuffer_.data_->time = static_cast<float>(Information::FrameTracker::GetInstance()->GetElapsedTimeS());
	commonBuffer_.data_->rWidth = Config::Window::kResolutionWidth;
	commonBuffer_.data_->rHeight = Config::Window::kResolutionHeight;
}

void PostProcessor::PreCommands(ID3D12GraphicsCommandList* list, Base::RenderResource* target) {
	for (auto& pass : passes_) {
		if (pass->use) {
			pass->PreCommand(list, target);
		}
	}
}

void PostProcessor::SetCommands(ID3D12GraphicsCommandList* list) {
	// RootSignatureとPSOをセット
	list->SetGraphicsRootSignature(root_);
	list->SetPipelineState(pso_.GetState());
	// 共通化したまとめて処理
	int offset = 2;
	list->SetGraphicsRootConstantBufferView(0, commonBuffer_.GetGPUView());
	for (auto& pass : passes_) {
		// 使用していればBind
		if (pass->use) {
			pass->BindCommand(list, &offset);
		}
	}
}

void PostProcessor::DebugGUI() {
	if (ImGui::TreeNode("PostProcess")) {
		ImGui::Checkbox("isActive", &isActive);
		
		if (ImGui::Button("Add Pass...")) {
			ImGui::OpenPopup("AddPassPopup");
		}
		
		if (ImGui::BeginPopup("AddPassPopup")) {
			if (ImGui::MenuItem("Bloom")) { AddPass<Bloom>(); }
			if (ImGui::MenuItem("OutLine")) { AddPass<OutLine>(); }
			if (ImGui::MenuItem("GrayScale")) { AddPass<GrayScale>(); }
			if (ImGui::MenuItem("Vignetting")) { AddPass<Vignetting>(); }
			if (ImGui::MenuItem("RadialBlur")) { AddPass<RadialBlur>(); }
			if (ImGui::MenuItem("RGBShift")) { AddPass<RGBShift>(); }
			if (ImGui::MenuItem("Glitch")) { AddPass<Glitch>(); }
			ImGui::EndPopup();
		}

		ImGui::Separator();

		for (int i = 0; i < passes_.size(); ++i) {
			ImGui::PushID(i);
			
			bool opened = ImGui::TreeNodeEx(passes_[i]->name.c_str(), ImGuiTreeNodeFlags_AllowItemOverlap);
			
			// 入れ替え・削除ボタン（ノードと同じ行に表示）
			ImGui::SameLine(ImGui::GetWindowWidth() - 100);
			if (ImGui::Button("U", ImVec2(20, 0)) && i > 0) {
				std::swap(passes_[i], passes_[i - 1]);
			}
			ImGui::SameLine();
			if (ImGui::Button("D", ImVec2(20, 0)) && i < passes_.size() - 1) {
				std::swap(passes_[i], passes_[i + 1]);
			}
			ImGui::SameLine();
			if (ImGui::Button("X", ImVec2(20, 0))) {
				passes_.erase(passes_.begin() + i);
				CreateShaderFile();	// 削除されたのでシェーダー更新
				ImGui::PopID();
				if (opened) ImGui::TreePop();
				break; 
			}

			if (opened) {
				passes_[i]->DebugGUI();
				ImGui::TreePop();
			}
			
			ImGui::PopID();
		}

		ImGui::TreePop();
	}
}

void PostProcessor::CreateShaderFile() {
	// ファイルストリームを作成
	std::ofstream shader;
	shader.open("resources/system/shaders/Rework_/postProcess/temp.PS.hlsl", std::ios::trunc);	// ファイルをオープン

	// 最初に必要な定義を行う
	shader << R"(
#include "../utility/Random.hlsli"

struct PSInput
{
    float32_t4 position : SV_POSITION;
    float32_t2 texcoord : TEXCOORD0;
};
struct Parameter
{
    float time;
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
	for (auto& pass : passes_) {
		// 使用するなら
		if (pass->use) {
			// 書き込み
			pass->WriteBinding(&shader, &root_, &b);
		}
	}
	// 最後にrootSignatureをビルド
	root_.Build();

	shader << R"(
float32_t4 main(PSInput input) : SV_TARGET {
	float32_t4 output;
	float2 uv = input.texcoord;

    output = gTexture.Sample(gSampler, uv);
)";

	// 処理の書き込み
	for (auto& pass : passes_) {
		// 使用するなら書き込み処理
		if (pass->use) {
			pass->WriteProcess(&shader);
		}
	}

	shader << R"(
	output.a = 1.0f;
	return output;
}
)";

	// シェーダー作成終了
	shader.close();
	// シェーダーファイルを元にPSOを作成
	pso_.Init(root_)
		.SetDepthState(false)
		.SetSystemVS("Rework_/postProcess/PassThrough.VS.hlsl")
		.SetSystemPS("Rework_/postProcess/temp.PS.hlsl")
		.Build();
}