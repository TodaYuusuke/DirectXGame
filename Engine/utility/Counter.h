#pragma once

// 今後の実装予定
// ・加算減算の呼び出し制限機能

namespace LWP::Utility {
	/// <summary>
	/// 数値のカウンタークラス
	/// </summary>
	class Counter {
	public: // ** メンバ関数 ** //

		Counter() = default;
		~Counter() = default;

		/// <summary>
		/// 使用済みのカウントをリセットする関数
		/// </summary>
		/// <returns></returns>
		void Reset() { count_ = 0; }

		/// <summary>
		/// 使用済みのカウントを受け取る関数
		/// </summary>
		/// <returns></returns>
		const int Get() { return count_; }

		/// <summary>
		/// 使用済みのカウントを１つ足す関数
		/// </summary>
		void Increment() { count_++; }

		/// <summary>
		/// 使用済みのカウントを受け取った後、１つ足す関数
		/// </summary>
		/// <returns></returns>
		const int GetAndIncrement() { return count_++; }

		/// <summary>
		/// 使用済みのカウントを１つ減らす関数
		/// </summary>
		void Decrement() { count_--; }

		/// <summary>
		/// 使用済みのカウントを受け取った後、１つ足す関数
		/// </summary>
		/// <returns></returns>
		const int GetAndDecrement() { return count_--; }

	private: // ** メンバ変数 ** //

		// 数値
		int count_ = 0;
	};
}