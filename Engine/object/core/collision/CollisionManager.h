#pragma once
#include "Collision.h"

#include "utility/IndexManager.h"
#include "utility/PtrManager.h"
#include <map>

namespace LWP::Object {
	/// <summary>
	/// 当たり判定用の管理クラス
	/// </summary>
	class CollisionManager {
	public: // ** パブリックなメンバ関数 ** //
		// コンストラクタ
		CollisionManager();
		// デストラクタ
		~CollisionManager() = default;

		/// <summary>
		/// 初期化
		/// </summary>
		void Initialize();

		/// <summary>
		/// 更新
		/// </summary>
		void Update();

		// インスタンスのポインタをセット（ユーザー呼び出し不要）
		void SetPointer(Collision* ptr);
		// インスタンスのポインタを解放（ユーザー呼び出し不要）
		void DeletePointer(Collision* ptr);


	private: // ** メンバ変数 ** //

		// 空間分割レベル
		uint32_t spaceDivisionLevel_ = 4;
		// 空間サイズ
		float spaceLength_ = 1000.0f;
		// 空間中心座標
		LWP::Math::Vector3 center_;

		// Colliderに割り振るシリアル番号管理
		Utility::IndexManager indexManager_;
		// コライダーのリスト
		Utility::PtrManager<Collision*> collisions_;


	public: // ** プライベートなメンバ関数 ** //

		// マスクチェック
		bool CheckMask(Collision* f, Collision* t);


	private:
		// ImGui用変数
		int selectedClass = 0;
		int currentItem = 0;
		// デバッグ用の生成したインスンタンスを格納しておく配列
		std::vector<Collision*> debugCols;
	public:
		/// <summary>
		/// Debug用GUI
		/// </summary>
		void DebugGUI();
	};
};