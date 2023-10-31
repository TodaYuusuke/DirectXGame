#pragma once
namespace LWP::Utility {
	/// <summary>
	/// 変数の変化を監視するオブザーバーデザインパターン用のクラス
	/// </summary>
	template <typename T>
	class Observer {
	public:
		// ** パブリックなメンバ変数 ** //
		
		// 変数
		T t;
		// 関数ポインタ
		//auto* funcPtr;


		// ** オペレーターオーバーロード ** //

		// Observer Add(+) T
		Observer operator+(const T& other) const;
		Observer& operator+=(const T& other);
		// Observer Subtract(-) T
		Observer operator-(const T& other) const;
		Observer& operator-=(const T& other);
		// Observer Multiply(*) T
		Observer operator*(const T& other) const;
		Observer& operator*=(const T& other);
		// Observer Division(/) T
		Observer operator/(const T& other) const;
		Observer& operator/=(const T& other);
		// Observer Equal(=) T
		Observer operator=(const T& other) const;
	};
}