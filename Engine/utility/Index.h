#pragma once
#include <functional>

namespace LWP::Utility {
	/// <summary>
	/// インデックスマネージャーとセットで使用するクラス
	/// </summary>
	class Index {
	public:

		/// <summary>
		/// デフォルトコンストラクタ（消去）
		/// </summary>
		Index() {
			func = nullptr;
			i = -1;
		}
		/// <summary>
		/// コンストラクタ
		/// </summary>
		Index(std::function<void(int)> f, int value) { 
			func = f;
			i = value;
		}
		/// <summary>
		/// デストラクタ
		/// </summary>
		~Index() { if (func) { func(i); }; }

		/// <summary>
		/// インデックスを取得
		/// </summary>
		/// <returns></returns>
		int Get() const { return i; }


	private: // ** メンバ変数 ** //
		// インデックス
		int i = -1;
		// 解放用関数ポインタ
		std::function<void(int)> func = nullptr;


	public: // ** オペレーターオーバーロード ** //
		// ムーブコンストラクタ
		Index(Index&& other) noexcept {
			func = std::exchange(other.func, nullptr);
			i = std::exchange(other.i, -1);
		}

		// ムーブ代入演算子
		Index& operator=(Index&& other) noexcept {
			if (this != &other) {
				// ムーブ代入の実装
				func = std::exchange(other.func, nullptr);
				i = std::exchange(other.i, -1);
			}
			return *this;
		}

		// int型への暗黙の変換演算子をオーバーロード
		operator int() const {
			return i;
		}


		// コピー操作を禁止
		Index(const Index&) = delete;
		Index& operator=(const Index&) = delete;
	};
}