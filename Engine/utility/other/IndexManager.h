#pragma once
#include "Index.h"
#include <vector>
#include <cassert>

namespace LWP::Utility {
	/// <summary>
	/// インデックスの空き状況を管理をするクラス
	/// <para>Use系関数は返り値のインデックスを自動的に使用済みにします。</para>
	/// <para>UnUse系関数は引数のインデックスを未使用にします</para>
	/// <para>Get系関数は値を返すのみで、とくに何もしない</para>
	/// </summary>
	class IndexManager {
	public:

		/// <summary>
		/// デフォルトコンストラクタ
		/// </summary>
		IndexManager() = delete;

		/// <summary>
		/// コンストラクタ
		/// </summary>
		IndexManager(int size);
		/// <summary>
		/// コンストラクタ
		/// </summary>
		IndexManager(int size, int offset);

		/// <summary>
		/// デストラクタ
		/// </summary>
		~IndexManager() = default;

		// 空きがあるかを確認する
		bool CheckEmpty();
		
		// 空き番号を使用する
		Index UseEmpty();

		// デバッグ用ImGui
		void DebugGUI();


	private: // ** プライベートな関数 ** //

		// 指定した番号を未使用にする
		void UnUse(int index);


	private: // ** メンバ変数 ** //
		// サイズをコンスト指定
		const int kSize;

		// 開始番号をずらすためのoffset
		int offset_ = 0;
		// 使用済み配列（false = 未使用、true = 使用済み）
		std::vector<bool> array;
	};
}