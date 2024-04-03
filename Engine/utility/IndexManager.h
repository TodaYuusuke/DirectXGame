#pragma once
#include "Index.h"
#include <vector>
#include <cassert>

#include "base/ImGuiManager.h"

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
		IndexManager(int size) : kSize(size) {
			// サイズを決定しすべて未使用（false）で初期化
			array.resize(kSize, false);
		};
		/// <summary>
		/// デストラクタ
		/// </summary>
		~IndexManager() = default;

		// 空きがあるかを確認する
		bool CheckEmpty() {
			for (int i = 0; i < kSize; i++) {
				// 未使用のものがあればtrueを返す
				if (!array[i]) {
					return true;
				}
			}

			// 無かったのでfalseを返す
			return false;
		}
		
		// 空き番号を使用する
		Index UseEmpty() {
			for (int i = 0; i < kSize; i++) {
				// 未使用のものがあれば処理を行う
				if (!array[i]) {
					// 使用済みに
					array[i] = true;
					// 自動解放機能付きのIndexクラスを返す
					return Index(
						[&](int i) { UnUse(i); },
						i
					);
				}
			}

			// 使用できなかったのでエラー
			assert(false);
			return Index(nullptr, -1);
		}

		// デバッグ用ImGui
		void DebugGUI() {
			for (int i = 0; i < kSize; i++) {
				ImGui::Text("%d", static_cast<int>(array[i]));

				// 10行ごとの場合改行
				if (((i + 1) % 10 == 0)) {
					continue;
				}
				// 5列目にハイフン
				else if ((i + 1) % 5 == 0) {
					ImGui::SameLine();
					ImGui::Text("-");
				}
				
				// 改行キャンセル
				ImGui::SameLine();
			}
		}

	private: // ** プライベートな関数 ** //

		// 指定した番号を未使用にする
		void UnUse(int index) {
			array[index] = false;
		}

	private: // ** メンバ変数 ** //
		// サイズをコンスト指定
		const int kSize;

		// 使用済み配列（false = 未使用、true = 使用済み）
		std::vector<bool> array;
	};
}