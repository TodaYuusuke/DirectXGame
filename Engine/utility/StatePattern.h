#pragma once
#include "MyString.h"
#include <array>
#include <functional>
#include <optional>
#include "base/ImGuiManager.h"

namespace LWP::Utility {
	/// <summary>
	/// ステートパターンを管理するクラス
	/// </summary>
	template <typename E, std::size_t N,
	typename = std::enable_if_t<std::is_enum_v<E>>>
	class StatePattern {
	public: // ** パブリックなメンバ変数 ** //

		/// <summary>
		/// 初期化関数群
		/// </summary>
		/// <para>前回の状態を貰うため(pre)の引数</para>
		std::array<std::function<void([[maybe_unused]] const E& pre)>, N> initFunction;
		/// <summary>
		/// 更新関数群
		/// </summary>
		/// <para>次の状態をリクエストするため(req)と、前回の状態を貰うため(pre)の引数</para>
		std::array<std::function<void([[maybe_unused]] std::optional<E>& req, [[maybe_unused]] const E& pre)>, N> updateFunction;
		
		/// <summary>
		/// 次の状態リクエスト
		/// </summary>
		std::optional<E> request = std::nullopt;

		/// <summary>
		/// ImGui用の文字列
		/// </summary>
		std::array<std::string, N> name;


	private: // ** メンバ変数 ** //

		// 現在の状態
		E current_;
		// 変更前の状態
		E pre_;


	public: // ** メンバ関数 ** //
		
		// コンストラクタ
		StatePattern() { Init(); }
		// デストラクタ
		~StatePattern() = default;

		/// <summary>
		/// 初期化
		/// </summary>
		void Init() {
			// 各変数を初期化
			request = std::nullopt;
			current_ = static_cast<E>(0);	// 0が初期値前提
			pre_ = static_cast<E>(0);
			// 関数ポインタに何もしない関数をデフォルトで設定
			for (int i = 0; i < N; i++) {
				initFunction[i] = [](const E& pre) { pre; };
				updateFunction[i] = [](std::optional<E>& req, const E& pre) { req; pre; };
				name[i] = std::to_string(i);
			}
		}
		/// <summary>
		/// 更新
		/// </summary>
		void Update() {
			// 状態リクエストがある時実行
			if (request) {
				pre_ = current_;	// 過去の状態を保存
				current_ = request.value();	// 状態を更新
				request = std::nullopt;	// リクエストを無に
				// 初期化処理
				initFunction[static_cast<int>(current_)](pre_);
			}
			// 状態更新処理
			updateFunction[static_cast<int>(current_)](request, pre_);
		}

		/// <summary>
		///	デバッグ用のImGui
		/// </summary>
		void DebugGUI() {
			// enumの文字列が取得できないのでいったん数字で表示する
			ImGui::Text(("Current State : " + name[static_cast<int>(current_)]).c_str());
			ImGui::Text(("Pre State : " + name[static_cast<int>(pre_)]).c_str());
			ImGui::Text("----- State Change -----");
			for (int i = 0; i < N; i++) {
				if (ImGui::Button(name[i].c_str())) { request = static_cast<E>(i); }
			}
		}

		/// <summary>
		/// 現在の状態を取得
		/// </summary>
		E GetCurrentBehavior() { return current_; }
	};
}