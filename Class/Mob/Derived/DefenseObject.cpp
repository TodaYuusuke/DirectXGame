#include "DefenseObject.h"

using namespace LWP::Primitive;
using namespace LWP::Utility;

/// 初期化
void DefenseObject::Initialize(){
	// 影
	shadow_ = CreateInstance<Surface>();
	shadow_->transform.translation.y = 0.01f;
	shadow_->transform.rotation.x = 1.57f;
	shadow_->transform.scale = { 0.125f, 0.125f, 0.125f };
	shadow_->commonColor = new Color(0x00000064);

	// モデル
	model_ = LWP::Resource::LoadModel("defense/defense.obj");
	model_->transform.scale = { 0.1f ,0.1f, 0.1f };
	//model_->commonColor = new Color(ColorPattern::BLUE);

	// サウンド
	fallSE_ = LWP::Resource::LoadAudio("efall01.wav");
	fallSE_->SetLoopCount(0);

	// HPを1000
	hp_ = 1000;
}