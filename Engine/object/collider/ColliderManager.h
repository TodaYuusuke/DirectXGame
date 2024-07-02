#pragma once
#include "Collider.h"

#include "utility/IndexManager.h"
#include "utility/PtrManager.h"
#include <map>

namespace LWP::Object::Collider {
	/// <summary>
	/// 当たり判定用の管理クラス
	/// </summary>
	class Manager {
	public: // ** パブリックなメンバ関数 ** //
		// コンストラクタ
		Manager();
		// デストラクタ
		~Manager() = default;

		/// <summary>
		/// 初期化
		/// </summary>
		void Initialize();

		/// <summary>
		/// 更新
		/// </summary>
		void Update();

		// インスタンスのポインタをセット（ユーザー呼び出し不要）
		void SetPointer(Collider* ptr);
		// インスタンスのポインタを解放（ユーザー呼び出し不要）
		void DeletePointer(Collider* ptr);


	private: // ** メンバ変数 ** //

		// Colliderに割り振るシリアル番号管理
		Utility::IndexManager indexManager_;
		// コライダーのリスト
		Utility::PtrManager<Collider*> colliders_;


	public: // ** プライベートなメンバ関数 ** //

		// マスクチェック
		bool CheckMask(Collider* f, Collider* t);


	private:
		// ImGui用変数
		int selectedClass = 0;
		int currentItem = 0;
		// デバッグ用の生成したインスンタンスを格納しておく配列
		std::vector<Collider*> debugPris;
	public:
		/// <summary>
		/// Debug用GUI
		/// </summary>
		void DebugGUI();
	};
};