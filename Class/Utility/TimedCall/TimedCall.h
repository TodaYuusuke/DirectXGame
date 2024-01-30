#pragma once
#include <functional>

template<class T>
// 時限発動クラス
class TimedCall {

public:
	
	// コンストラクタ
	TimedCall(std::function<T> function, uint32_t time) : callBack_(function) {
		time_ = time;
		isFinished_ = false;
	}
	// 更新
	void Update() {
		if (isFinished_) {
			return;
		}

		if (--time_ <= 0) {
			// コールバック関数呼び出し
			callBack_();
			// 完了フラグをtrueに
			isFinished_ = true;
		}
	}
	// 完了ならばtrueを返す
	bool IsFinished() { return isFinished_; }

private: // メンバ変数

	// コールバック
	std::function<T> callBack_;
	// 残り時間
	uint32_t time_;
	// 完了フラグ
	bool isFinished_;

};