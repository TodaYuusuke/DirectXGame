#pragma once
namespace LWP::Utility {
	/// <summary>
	/// 変数の変化を監視するオブザーバーデザインパターン用のクラス
	/// <para>テンプレートに登録するには、==の比較演算の定義が必要</para>
	/// </summary>
	template <typename T>
	class Observer {
	public:	// ** パブリックなメンバ変数 ** //
		
		// 変数
		T t;

		// 初期化
		void Init(T value) {
			t = value;
			preT = value;
		}

		// 変化したかのフラグを返す関数
		bool GetChanged() {
			bool result = !(t == preT);
			preT = t; // チェックしたので戻す
			return result;
		}

		// コンストラクタ
		Observer() = default;
		Observer(T initValue) {
			t = initValue;
			preT = initValue;
		}

		// ** オペレーターオーバーロード ** //
#pragma region オペレーターオーバーロード
		// Observer Add(+) T
		Observer operator+(const T& other) const {
			return Observer(t + other);
		}
		Observer& operator+=(const T& other) {
			t += other;
			return *this;
		}

		// Observer Subtract(-) T
		Observer operator-(const T& other) const {
			return Observer(t - other);
		}
		Observer& operator-=(const T& other) {
			t -= other;
			return *this;
		}
		
		// Observer Multiply(*) T
		Observer operator*(const T& other) const {
			return Observer(t * other);
		}
		Observer& operator*=(const T& other) {
			t *= other;
			return *this;
		}
		
		// Observer Division(/) T
		Observer operator/(const T& other) const {
			return Observer(t / other);
		}
		Observer& operator/=(const T& other) {
			t /= other;
			return *this;
		}
		
		// Observer Equal(=) T
		// 代入されるとき
		Observer& operator=(const T& other) {
			t = other;
			return *this;
		}
		// 代入するとき
		Observer& operator=(const Observer& other) {
			t = other.t;
			return *this;
		};
		// Tに代入するとき
		operator T() const {
			return t;
		};

		// 前置インクリメント
		Observer& operator++() {
			++t;
			return *this;
		}
		// 後置インクリメント
		Observer operator++(int) {
			Observer temp = *this;
			++t;
			return temp;
		}
		// 前置デクリメント
		Observer& operator--() {
			--t;
			return *this;
		}
		// 後置デクリメント
		Observer operator--(int) {
			Observer temp = *this;
			--t;
			return temp;
		}
#pragma endregion

	private: // ** メンバ変数 ** //

		// 変化前の値を保持する変数
		T preT;
	};

	/// <summary>
	/// 変数の変化を監視するオブザーバーデザインパターン用のクラス（特殊化：ポインタ用）
	/// <para>テンプレートに登録するには、==の比較演算の定義が必要</para>
	/// <para>デフォルトコンストラクタの定義も必要</para>
	/// <para>純粋仮想関数を持っている場合は実体が作れないので無理！</para>
	/// <para>純粋仮想関数を持っている場合の回避バージョンはそのうち作る</para>
	/// </summary>
	template <typename T>
	class Observer<T*> {
	public:	// ** パブリックなメンバ変数 ** //

		// 変数
		T* t;

		// 初期化
		void Init(T* value) {
			t = value;
			if (t) { preT = *t; }
		}

		// 変化したかのフラグを返す関数
		bool GetChanged() {
			// テンプレートがポインタの時の処理
			bool result = !(*t == preT);
			preT = *t; // チェックしたので戻す
			return result;
		}

		// コンストラクタ
		Observer() = default;
		Observer(T* initValue) { Init(initValue); }

	private: // ** メンバ変数 ** //

		// 変化前の値を保持する変数
		T preT;
	};
}