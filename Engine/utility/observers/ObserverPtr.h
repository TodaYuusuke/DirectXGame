#pragma once
#include <type_traits>

namespace LWP::Utility {
	/// <summary>
	/// 変数の変化を監視するオブザーバーデザインパターン用のクラス（ポインタ監視用）
	/// <para>アドレス自体の変化を監視する用のクラス</para>
	/// <para>==の比較演算の定義が必要かも（要検証）</para>
	/// </summary>
	template <typename pointer>
	concept Pointer = std::is_pointer_v<pointer>;
	template <Pointer T>
	class ObserverPtr {
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
			preT = t; // チェックしたので戻す
			return result;
		}

		// コンストラクタ
		ObserverPtr() = default;
		ObserverPtr(T initValue = nullptr) {
			t = initValue;
			preT = t;
		}

		// ** オペレーターオーバーロード ** //
#pragma region オペレーターオーバーロード
		// Observer Equal(=) T
		// 代入されるとき
		ObserverPtr& operator=(const T& other) {
			t = other;
			return *this;
		}
		// 代入するとき
		ObserverPtr& operator=(const ObserverPtr& other) {
			t = other.t;
			return *this;
		};
		// Tに代入するとき
		operator T() const {
			return t;
		};

		// 前置インクリメント
		ObserverPtr& operator++() {
			++t;
			return *this;
		}
		// 後置インクリメント
		ObserverPtr operator++(int) {
			ObserverPtr temp = *this;
			++t;
			return temp;
		}
		// 前置デクリメント
		ObserverPtr& operator--() {
			--t;
			return *this;
		}
		// 後置デクリメント
		ObserverPtr operator--(int) {
			ObserverPtr temp = *this;
			--t;
			return temp;
		}
#pragma endregion

	private: // ** メンバ変数 ** //

		// 変化前の値を保持する変数
		T preT;
	};
}