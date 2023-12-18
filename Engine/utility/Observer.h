#pragma once
namespace LWP::Utility {
	/// <summary>
	/// 変数の変化を監視するオブザーバーデザインパターン用のクラス
	/// </summary>
	template <typename T>
	class Observer {
	public:	// ** パブリックなメンバ変数 ** //
		
		// 変数
		T t;

		// 変化したかのフラグを返す関数
		bool GetChanged() {
			bool result = isChanged_;
			isChanged_ = false; // チェックしたので戻す
			return result;
		}

		// コンストラクタ
		Observer(T initValue) {
			t = initValue;
		};

		// ** オペレーターオーバーロード ** //

		// Observer Add(+) T
		Observer operator+(const T& other) const {
			return t + t;
		}
		Observer& operator+=(const T& other) {
			isChanged_ = true;
			return *this = *this + t;
		}

		// Observer Subtract(-) T
		Observer operator-(const T& other) const {
			return t - t;
		}
		Observer& operator-=(const T& other) {
			isChanged_ = true;
			return *this = *this - t;
		}
		
		// Observer Multiply(*) T
		Observer operator*(const T& other) const {
			return t * t;
		}
		Observer& operator*=(const T& other) {
			isChanged_ = true;
			return *this = *this * t;
		}
		
		// Observer Division(/) T
		Observer operator/(const T& other) const {
			return t / t;
		}
		Observer& operator/=(const T& other) {
			isChanged_ = true;
			return *this = *this / t;
		}
		
		// Observer Equal(=) T
		Observer& operator=(const T& other) {
			isChanged_ = true;
			t = other;
			return *this;
		}
		Observer& operator=(const Observer& other) const = delete;
		Observer& operator=(const Observer&& other) const = delete;

	private: // ** メンバ変数 ** //

		// 変化したかを保持するフラグ
		bool isChanged_ = false;
	};
}