#pragma once
namespace LWP::Utility {
	/// <summary>
	/// 変数の変化を監視するオブザーバーデザインパターン用のクラス（構造体指定）
	/// <para>比較したいデータの構造体を定義すること</para>
	/// <para>==の比較演算と構造体への代入演算の定義が必要</para>
	/// <para>必要なメンバ変数を持った構の代入演算子のオーバーロードを定義すること</para>
	/// </summary>
	template <typename T, typename U>
	class Observer2 {
	public:	// ** パブリックなメンバ変数 ** //

		// 変数
		T t;

		// 初期化
		void Init(T value) {
			t = value;
			preT = t;
		}

		// 変化したかのフラグを返す関数
		bool GetChanged() {
			// テンプレートがポインタの時の処理
			bool result = !(t == preT);
			preT = *t; // チェックしたので戻す
			return result;
		}

		// コンストラクタ
		Observer2() = default;
		Observer2(T initValue) {
			// テンプレートがポインタの時の処理
			t = initValue;
			preT = t;
		}

		// ** オペレーターオーバーロード ** //
#pragma region オペレーターオーバーロード
		// Observer2 Add(+) T
		Observer2 operator+(const T& other) const {
			return Observer2(t + other);
		}
		Observer2& operator+=(const T& other) {
			t += other;
			return *this;
		}

		// Observer2 Subtract(-) T
		Observer2 operator-(const T& other) const {
			return Observer2(t - other);
		}
		Observer2& operator-=(const T& other) {
			t -= other;
			return *this;
		}

		// Observer2 Multiply(*) T
		Observer2 operator*(const T& other) const {
			return Observer2(t * other);
		}
		Observer2& operator*=(const T& other) {
			t *= other;
			return *this;
		}

		// Observer2 Division(/) T
		Observer2 operator/(const T& other) const {
			return Observer2(t / other);
		}
		Observer2& operator/=(const T& other) {
			t /= other;
			return *this;
		}

		// Observer2 Equal(=) T
		// 代入されるとき
		Observer2& operator=(const T& other) {
			t = other;
			return *this;
		}
		// 代入するとき
		Observer2& operator=(const Observer2& other) {
			t = other.t;
			return *this;
		};
		// Tに代入するとき
		operator T() const {
			return t;
		};

		// 前置インクリメント
		Observer2& operator++() {
			++t;
			return *this;
		}
		// 後置インクリメント
		Observer2 operator++(int) {
			Observer2 temp = *this;
			++t;
			return temp;
		}
		// 前置デクリメント
		Observer2& operator--() {
			--t;
			return *this;
		}
		// 後置デクリメント
		Observer2 operator--(int) {
			Observer2 temp = *this;
			--t;
			return temp;
		}
#pragma endregion

	private: // ** メンバ変数 ** //

		// 変化前の値を保持する変数
		U preT;
	};


	/// <summary>
	/// 変数の変化を監視するオブザーバーデザインパターン用のクラス
	/// 変数の変化を監視するオブザーバーデザインパターン用のクラス（構造体指定、特殊化：ポインタ用）
	/// <para>比較したいデータの構造体を定義すること</para>
	/// <para>==の比較演算と構造体への代入演算の定義が必要</para>
	/// <para>必要なメンバ変数を持った構の代入演算子のオーバーロードを定義すること</para>
	/// </summary>
	template <typename T, typename U>
	class Observer2<T*, U> {
	public:	// ** パブリックなメンバ変数 ** //

		// 変数
		T* t;

		// 初期化
		void Init(T* value) {
			t = value;
			preT = *t;
		}

		// 変化したかのフラグを返す関数
		bool GetChanged() {
			// テンプレートがポインタの時の処理
			bool result = !(*t == preT);
			preT = *t; // チェックしたので戻す
			return result;
		}

		// コンストラクタ
		Observer2() = default;
		Observer2(T* initValue) {
			// テンプレートがポインタの時の処理
			t = initValue;
			preT = *t;
		}

		// ** オペレーターオーバーロード ** //
#pragma region オペレーターオーバーロード
		// Observer2 Add(+) T
		Observer2 operator+(const T& other) const {
			return Observer2(t + other);
		}
		Observer2& operator+=(const T& other) {
			t += other;
			return *this;
		}

		// Observer2 Subtract(-) T
		Observer2 operator-(const T& other) const {
			return Observer2(t - other);
		}
		Observer2& operator-=(const T& other) {
			t -= other;
			return *this;
		}

		// Observer2 Multiply(*) T
		Observer2 operator*(const T& other) const {
			return Observer2(t * other);
		}
		Observer2& operator*=(const T& other) {
			t *= other;
			return *this;
		}

		// Observer2 Division(/) T
		Observer2 operator/(const T& other) const {
			return Observer2(t / other);
		}
		Observer2& operator/=(const T& other) {
			t /= other;
			return *this;
		}

		// Observer2 Equal(=) T
		// 代入されるとき
		Observer2& operator=(const T& other) {
			t = other;
			return *this;
		}
		// 代入するとき
		Observer2& operator=(const Observer2& other) {
			t = other.t;
			return *this;
		};
		// Tに代入するとき
		operator T() const {
			return t;
		};

		// 前置インクリメント
		Observer2& operator++() {
			++t;
			return *this;
		}
		// 後置インクリメント
		Observer2 operator++(int) {
			Observer2 temp = *this;
			++t;
			return temp;
		}
		// 前置デクリメント
		Observer2& operator--() {
			--t;
			return *this;
		}
		// 後置デクリメント
		Observer2 operator--(int) {
			Observer2 temp = *this;
			--t;
			return temp;
		}
#pragma endregion

	private: // ** メンバ変数 ** //

		// 変化前の値を保持する変数
		U preT;
	};
}