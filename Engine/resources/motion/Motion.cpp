#include "Motion.h"

#include "component/Resource.h"
#include "component/Information.h"
#include <algorithm>

using namespace LWP::Resource;
using namespace LWP::Utility;
using namespace LWP::Math;

Motion::Motion() {
	// インスタンスをエンジンに送る
	SetInstance(this);
}

Motion& Motion::Add(Vector3* target, const Vector3& move,
	const float& startSec, const float& durationSec,
	Easing::Type easingType, Interp::Type interType) {
	return Add({
		target,
		*target,	// 初期値
		*target + move,	// 終了値
		startSec,
		durationSec,
		easingType,
		interType,
		false
		});
}
Motion& Motion::Add(MotionData data) {
	data_.push_back(data);
	return *this;
}

Motion& Motion::DisableDeltaTimeMultiply() {
	isUseDeltaTimeMultiply_ = false;
	return *this;
}

void Motion::Start() { Start(0.0f); }
void Motion::Start(float startSec) {
	isStart_ = true;
	currentSec_ = startSec;

	// すべてisEndをfalseに
	for (MotionData& d : data_) {
		d.isEnd = false;
	}
}

void Motion::Update() {
	if (!isStart_) { return; }	// 早期リターン

	// 時間を更新
	currentSec_ += isUseDeltaTimeMultiply_ ? Info::GetDeltaTimeF() : Info::GetDefaultDeltaTimeF();

	// １つでもアニメーションが実行されたかフラグ
	bool isExecute = false;

	for (MotionData& d : data_) {
		// 終了済みなら早期コンティニュー
		if (d.isEnd) { continue; }

		// 0を基準にした経過時間（現在の秒 - 開始する秒）/ 終了までの時間（durationSec）= 0 ~ 1のt
		float t = easingFunc[static_cast<int>(d.easingType)]((currentSec_ - d.startSec) / d.durationSec);
		// ターゲットに書き込む
		*d.target = interpFunc[static_cast<int>(d.interType)](d.start, d.end, t);

		// tが1なら終了
		d.isEnd = static_cast<int>(t);	// 小数点以下切り捨て
		// 実行したフラグをtrueに
		isExecute = true;
	}

	// 一度も実行されなかったので、終了処理（if使う必要すらなさそうなんでこれで）
	isStart_ = isExecute;
}

bool Motion::isEnd() { return !isStart_; }


// -- 静的メンバ変数宣言 -- //

// イージング関数ポインタ
std::function<float(float)> Motion::easingFunc[] = {
	&Easing::Liner,
	&Easing::InQuint,
	&Easing::OutQuint,
	&Easing::InOutQuint,
	&Easing::InCirc,
	&Easing::OutCirc,
	&Easing::InOutCirc,
	&Easing::InCubic,
	&Easing::OutCubic,
	&Easing::InOutCubic,
	&Easing::InBack,
	&Easing::OutBack,
	&Easing::InOutBack,
	&Easing::InBounce,
	&Easing::OutBounce,
	&Easing::InOutBounce,
	&Easing::InElastic,
	&Easing::OutElastic,
	&Easing::InOutElastic,
	&Easing::InQuart,
	&Easing::OutQuart,
	&Easing::InOutQuart,
	&Easing::InExpo,
	&Easing::OutExpo,
	&Easing::InOutExpo,
};
// 補間関数ポインタ
std::function<Vector3(const Vector3&, const Vector3&, const float&)> Motion::interpFunc[] = {
	&Interp::Lerp,
	&Interp::Slerp,
};