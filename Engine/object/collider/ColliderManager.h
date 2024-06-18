#pragma once
#include "cAABB.h"
//#include "cOBB.h"
#include "cSphere.h"
#include "cCapsule.h"

#include "utility/PtrManager.h"
#include <map>

namespace LWP::Object::Collider {
	/// <summary>
	/// 当たり判定用の管理クラス
	/// </summary>
	class Manager {
	public: // ** パブリックなメンバ関数 ** //
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
		void Update();

		// インスタンスのポインタをセット（ユーザー呼び出し不要）
		void SetPointer(ICollider* ptr);
		// インスタンスのポインタを解放（ユーザー呼び出し不要）
		void DeletePointer(ICollider* ptr);


	private: // ** メンバ変数 ** //

		// コライダーのリスト
		Utility::PtrManager<ICollider*> colliders_;
		// カウンター
		int colliderCount_ = 0;
		
		// 関数ポインタの型
		using CollisionFunction = std::function<bool(ICollider*, ICollider*)>;
		CollisionFunction checkCollisions_[4][4];	// Shape::Countの値がサイズだよ！


	public: // ** プライベートなメンバ関数 ** //

		// マスクチェック
		bool CheckMask(ICollider* f, ICollider* t);

		// 各形状同士の当たり判定
		bool CheckCollision(AABB* c1, AABB* c2);
		//bool CheckCollision(AABB* c1, OBB* c2);
		bool CheckCollision(AABB* c1, Sphere* c2);
		bool CheckCollision(AABB* c1, Capsule* c2);
		//bool CheckCollision(OBB* c1, AABB* c2) { return CheckCollision(c2, c1); }
		//bool CheckCollision(OBB* c1, OBB* c2);
		//bool CheckCollision(OBB* c1, Sphere* c2);
		//bool CheckCollision(OBB* c1, Capsule* c2);
		bool CheckCollision(Sphere* c1, AABB* c2) { return CheckCollision(c2, c1); }
		//bool CheckCollision(Sphere* c1, OBB* c2) { return CheckCollision(c2, c1); }
		bool CheckCollision(Sphere* c1, Sphere* c2);
		bool CheckCollision(Sphere* c1, Capsule* c2);
		bool CheckCollision(Capsule* c1, AABB* c2) { return CheckCollision(c2, c1); }
		//bool CheckCollision(Capsule* c1, OBB* c2) { return CheckCollision(c2, c1); }
		bool CheckCollision(Capsule* c1, Sphere* c2) { return CheckCollision(c2, c1); }
		bool CheckCollision(Capsule* c1, Capsule* c2);


	private:
		// ImGui用変数
		int selectedClass = 0;
		int currentItem = 0;
		// デバッグ用の生成したインスンタンスを格納しておく配列
		std::vector<ICollider*> debugPris;
	public:
		/// <summary>
		/// Debug用GUI
		/// </summary>
		void DebugGUI();
	};
};