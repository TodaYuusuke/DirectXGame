#include "DirectXCommon.h"
#include "object/core/Camera.h"
#include "../utility/MyUtility.h"


#include <cassert>
#include <format>

using namespace Microsoft::WRL;
using namespace LWP::Base;
using namespace LWP::Utility;

DirectXCommon::DirectXCommon() {}

void DirectXCommon::Init() {
	renderingPipeline_ = std::make_unique<RenderingPipeline>();
	renderingPipeline_->Init();
}

//void DirectXCommon::SetMainCamera(Object::Camera* camera) {
//	// ポストプロセスを行うか確認
//	if (camera->pp.use) {
//		// カメラのリソースに書き込み
//		//renderer_->AddTarget(camera->GetBufferView(), camera->GetRenderResource(), &depthStencil_);
//		// ポストプロセス用のターゲットセット
//		//renderer_->AddTarget(camera->GetRenderResource(), &backBuffers_[swapChain_->GetCurrentBackBufferIndex()], &depthStencil_, &camera->pp);
//	}
//	// しないならば
//	else {
//		// 直接BackBufferに書き込み
//		//renderer_->AddTarget(camera->GetBufferView(), &backBuffers_[swapChain_->GetCurrentBackBufferIndex()], &depthStencil_);
//	}
//
//	// SpriteをBackBufferに
//	//renderer_->AddSpriteTarget(camera->GetBufferView(), &backBuffers_[swapChain_->GetCurrentBackBufferIndex()], &depthStencil_);
//}

void DirectXCommon::DrawCall() {
	renderingPipeline_->DrawCall();
}

void DirectXCommon::DebugGUI() {
	if (ImGui::BeginTabItem("Base")) {
		RTV::GetInstance()->DebugGUI();
		SRV::GetInstance()->DebugGUI();
		DSV::GetInstance()->DebugGUI();
		ImGui::EndTabItem();
	}
}