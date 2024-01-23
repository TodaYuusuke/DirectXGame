#include "LensDistortion.h"

using namespace LWP::Base::PostProcess;
constexpr int kMax = LWP::Config::PostProcess::kMaxLensDistortion;	// 最大値を省略

void LensDistortionRenderer::Init() {
	// リソースを生成
}
void LensDistortionRenderer::SetRenderData(RenderResource* target, LensDistortion data) {
	target;
	data;

}
void LensDistortionRenderer::Clear() {

}