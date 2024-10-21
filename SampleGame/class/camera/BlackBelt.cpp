#include "BlackBelt.h"

using namespace LWP;
using namespace LWP::Math;
using namespace LWP::Input;
using namespace LWP::Object;
using namespace LWP::Resource;
using namespace LWP::Utility;
using namespace LWP::Info;

// 初期化
void BlackBelt::Init() {
	for (int i = 0; i < 2; i++) {
		blackBelt_[i].material.color = ColorPattern::BLACK;
		blackBelt_[i].worldTF.scale = { 9.6f, 0.0f, 1.0f };
		blackBelt_[i].name = "BlackBelt" + std::to_string(i);
	}

	// 下帯の座標を変える
	blackBelt_[1].worldTF.translation.y = 1080.0f;
	blackBelt_[1].anchorPoint = { 0.0f,1.0f };
}

void BlackBelt::Update() {
	// 黒帯がオンならアニメーションを進行
	if (isActive_) { time_ += GetDeltaTimeF(); }
	// そうでないならアニメーションは退行
	else { time_ -= GetDeltaTimeF(); }

	time_ = std::clamp(time_, 0.0f, 1.0f);	// 0~1にクランプ

	for (int i = 0; i < 2; i++) {
		blackBelt_[i].worldTF.scale.y = Interp::LerpF(0.0f, scale_, Easing::OutQuint(time_));
	}
}
