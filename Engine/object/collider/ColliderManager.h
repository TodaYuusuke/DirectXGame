#pragma once
#include "cAABB.h"
//#include "cOBB.h"
#include "cSphere.h"
#include "cCapsule.h"

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
				
		/// <summary>
		/// インスタンスを登録する
		/// </summary>
		/// <param name="object">登録するオブジェクト</param>
		template <IsICollider TCollider>
		inline TCollider* CreateInstance() {
			TCollider* newObject = new TCollider();

#if DEMO
			// typeid を使用して型情報を取得
			const std::type_info& typeInfo = typeid(TCollider);
			// type_info オブジェクトからクラス名を取得
			std::string className = typeInfo.name();
			// 名前空間部分を削除（LWP::Object::Collider::）
			className = className.erase(0, 29);

			// カウントのマップから数を測定し、デフォルトの名前を登録
			if (!colliderCountMap_.count(className)) {
				// 存在しない場合のみ0で初期化
				colliderCountMap_[className] = 0;
			}
			newObject->name = className + "_" + std::to_string(colliderCountMap_[className]++);
#endif

			colliders_.push_back(newObject);
			return newObject;
		}

	private: // ** メンバ変数 ** //

		// コライダーのリスト
		std::vector<ICollider*> colliders_;

		// 関数ポインタの型
		using CollisionFunction = std::function<bool(ICollider*, ICollider*)>;
		CollisionFunction checkCollisions_[4][4];	// Shape::Countの値がサイズだよ！

#if DEMO
		// インスタンスカウント用マップ
		std::map<std::string, int> colliderCountMap_;
		// ImGui用変数
		int selectedClass = 0;
		int currentItem = 0;
#endif

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

	};
};