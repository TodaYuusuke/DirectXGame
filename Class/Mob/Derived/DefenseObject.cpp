#include "DefenseObject.h"

using namespace LWP::Primitive;
using namespace LWP::Utility;

/// 初期化
void DefenseObject::Initialize(){
	// モデル
	model_ = LWP::Resource::LoadModel("defense/defense.obj");
	model_->transform.scale = { 0.1f ,0.1f, 0.1f };
	model_->material.enableLighting = true;
	//model_->commonColor = new Color(ColorPattern::BLUE);
}