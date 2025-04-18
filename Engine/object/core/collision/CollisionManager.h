#pragma once
#include "Collision.h"

#include "utility/IndexManager.h"
#include "utility/PtrManager.h"
#include "utility/Singleton.h"
#include <map>

namespace LWP::Object {
	/// <summary>
	/// 当たり判定用の管理クラス
	/// </summary>
	class CollisionManager : public Utility::ISingleton<CollisionManager> {
		friend class Utility::ISingleton<CollisionManager>;	// ISingletonをフレンドクラスにしてコンストラクタを呼び出せるように
	private: // ** シングルトン化に必要な処理 ** //

		/// <summary>
		/// コンストラクタをプライベートに
		/// </summary>
		CollisionManager();

	public:	// ** メンバ関数 ** //

		/// <summary>
		/// 初期化
		/// </summary>
		void Init();

		/// <summary>
		/// 更新
		/// </summary>
		void Update();

		// インスタンスのポインタをセット（ユーザー呼び出し不要）
		void SetPointer(Collision* ptr);
		// インスタンスのポインタを解放（ユーザー呼び出し不要）
		void DeletePointer(Collision* ptr);

		/// <summary>
		/// Debug用GUI
		/// </summary>
		void DebugGUI();

	private: // ** メンバ変数 ** //

		// オクトツリー
		Object::OctreeSpaceDivision octree_;

		// Colliderに割り振るシリアル番号管理
		Utility::IndexManager indexManager_;
		// コライダーのリスト
		Utility::PtrManager<Collision*> collisions_;


	private:
		// ImGui用変数
		int selectedClass = 0;
		int currentItem = 0;
		// デバッグ用の生成したインスンタンスを格納しておく配列
		std::vector<Collision*> debugCols;
	private: // ** プライベートなメンバ関数 ** //
	};
};