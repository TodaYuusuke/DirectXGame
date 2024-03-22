#pragma once
#include <list>
#include <cassert>

namespace LWP::Utility {
	template <typename P>
	concept PP = std::is_pointer_v<P>;
	template <PP T>
	/// <summary>
	/// 生成されたインスタンスのpointerを管理するクラス
	/// </summary>
	class PtrManager {
	public:
		// ポインターのlist
		std::list<T> list;


		/// <summary>
		/// コンストラクタ
		/// </summary>
		PtrManager() = default;
		/// <summary>
		/// デストラクタ
		/// </summary>
		~PtrManager() = default;

		/// <summary>
		/// ポインタをセットする
		/// </summary>
		void SetPointer(T ptr) {
			list.push_back(ptr);
		}
		/// <summary>
		/// ポインタを解放する
		/// </summary>
		void DeletePointer(T ptr) {
			for (typename std::list<T>::iterator i = list.begin(); i != list.end(); i++) {
				// 一致するアドレスを検索
				if (*i == ptr) {
					list.erase(i);
					return;
				}
			}
			// 存在しなかったのでエラー
			assert(false);
		}
	};
}