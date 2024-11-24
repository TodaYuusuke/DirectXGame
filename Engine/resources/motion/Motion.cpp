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
Motion::~Motion() {
	// ここでどうにかしてエンジン上のポインタを破棄させたい！
	DeleteInstance(this);
}

Motion& Motion::Add(Vector3* target, const Vector3& move,
	const float& startSec, const float& durationSec,
	Easing::Type easingType, Interp::Type interType) {
	return Add({
		target,
		nullptr,
		move,
		startSec,
		durationSec,
		easingType,
		interType,
		false
		});
}

Motion& Motion::DisableDeltaTimeMultiply() {
	isUseDeltaTimeMultiply_ = false;
	return *this;
}

void Motion::Start() { Start(0.0f); }
void Motion::Start(bool loopFlag) { Start(loopFlag, 0.0f); }
void Motion::Start(float startSec) { Start(false, startSec); }
void Motion::Start(bool loopFlag, float startSec) {
	loopFlag_ = loopFlag;
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

	do {
		// 実行し終わったモーションをカウント
		int executed = 0;
		for (MotionData& d : data_) {
			// 終了済み または 開始秒数より前ならば早期コンティニュー
			if (d.isEnd || currentSec_ < d.startSec) {
				executed++;	// 終了済みならカウント
				continue;
			}
			// スタート地点を生成
			if (!d.start) { d.start = new Vector3(*d.target); }

			// 0を基準にした経過時間（現在の秒 - 開始する秒）/ 終了までの時間（durationSec）= 0 ~ 1のt
			float t = easingFunc[static_cast<int>(d.easingType)]((currentSec_ - d.startSec) / d.durationSec);
			// ターゲットに書き込む
			*d.target = interpFunc[static_cast<int>(d.interType)](*d.start, d.End(), t);

			// tが1なら終了
			if (static_cast<int>(t) >= 1.0f) {
				d.isEnd = true;
				executed++;
			}
		}

		// ループフラグによって処理が変わる
		if (loopFlag_) {
			// すべて実行されているならば初期化してもう一度処理を行う
			if (executed >= data_.size()) {
				executed = 0;
				for (MotionData& d : data_) { d.isEnd = false; }	// フラグをオフに
				currentSec_ = std::fmod(currentSec_, totalTime_);	// 0まで戻してループ
				continue;
			}
		}
		else {
			// すべて実行され終わったので終了
			isStart_ = !(executed >= data_.size());
		}
	} while (false);	// ループフラグでもう一度戻る用
}

bool Motion::isEnd() { return !isStart_; }

Motion& Motion::Add(MotionData data) {
	data_.push_back(data);
	// モーション全体の時間を求める
	float motionTime = data.durationSec + data.startSec;
	if (motionTime > totalTime_) { totalTime_ = motionTime; }
	return *this;
}

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