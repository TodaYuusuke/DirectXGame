#pragma once
#include "core/CoreList.h"
#include "utility/PtrManager.h"

#include <vector>
#include <map>
#include <typeinfo>

namespace LWP::Object {
	/// <summary>
	/// システム用のオブジェクトをまとめて管理するクラス
	/// </summary>
	class Manager {
	public: // メンバ関数

		// コンストラクタ
		Manager() = default;
		// デストラクタ
		~Manager() = default;

		/// <summary>
		/// 初期化
		/// </summary>
		void Initialize();

		/// <summary>
		/// 更新
		/// </summary>
		void Update(Base::RendererManager* manager);

		// インスタンスのポインタをセット（ユーザー呼び出し不要）
		void SetPointer(IObject* ptr);
		// インスタンスのポインタを解放（ユーザー呼び出し不要）
		void DeletePointer(IObject* ptr);


	private: // メンバ変数

		// オブジェクトのリスト
		Utility::PtrManager<IObject*> objects_;
		// カウンター
		int objectCount_ = 0;

#if DEMO
		// ImGui用変数
		int selectedClass = 0;
		int currentItem = 0;
		// デバッグ用の生成したインスンタンスを格納しておく配列
		std::vector<IObject*> debugPris;
#endif
	};
}