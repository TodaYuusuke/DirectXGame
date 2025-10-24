#pragma once
#include "child/IncludeChild.h"
#include "functions/IncludeFunctions.h"
#include "motionEffects/IncludeEffects.h"
#include "observers/IncludeObservers.h"
#include "other/IncludeOthers.h"

namespace LWP::Utility {
	/// <summary>
	/// インスタンス管理が必要な便利系クラス管理
	/// </summary>
	class Manager : public ISingleton<Manager> {
		friend class ISingleton<Manager>;	// ISingletonをフレンドクラスにしてコンストラクタを呼び出せるように
	private: // ** シングルトン化に必要な処理 ** //

		/// <summary>
		/// コンストラクタをプライベートに
		/// </summary>
		Manager();


	public: // ** メンバ関数 ** //

		/// <summary>
		/// 初期化
		/// </summary>
		void Init();

		/// <summary>
		/// 更新
		/// </summary>
		void Update();

		/// <summary>
		/// Debug用のImGUI
		/// </summary>
		void DebugGUI();

		// インスタンスのポインタをセット（ユーザー呼び出し不要）
		void SetPtr(IUtility* ptr) {
			ptr->name += "_" + std::to_string(count_++);	// 数字だけ渡す
			instances_.SetPtr(ptr);
		}
		// インスタンスのポインタを解放（ユーザー呼び出し不要）
		void DeletePtr(IUtility* ptr) { instances_.DeletePtr(ptr); }


	private: // ** メンバ変数 ** //

		// カウンター
		int count_ = 0;

		enum PlaneRenderType {
			Sprite,
			Billboard,
			Count
		};

		// ポインタのリスト
		PtrManager<IUtility*> instances_;
		// デバッグ用の生成したインスンタンスを格納しておく配列
		std::vector<IUtility*> debugInstance_;
	};
}