#pragma once
#include "object/TransformQuat.h"

namespace LWP::Object::Collider {
	// 識別子
	enum class Shape : int {
		AABB = 0,
		OBB = 1,
		Sphere = 2,
		Capsule = 3,
		Count	// カウント
	};

	// 前方宣言
	class AABB;
	//class OBB;
	class Sphere;
	class Capsule;


	/// <summary>
	/// 当たり判定の形状用の基底クラス
	/// </summary>
	class ICollisionShape {
	public: // ** パブリックなメンバ変数 ** //

		// アクティブ切り替え
		bool isActive = true;
#if DEMO
		// ** デバッグ用の変数 ** //
		bool isShowWireFrame = true;
#endif

	protected: // ** プライベートなメンバ変数 ** //

		// Colliderのトランスフォームのポインタ
		Object::TransformQuat* follow_ = nullptr;


	public: // ** メンバ関数 ** //

		/// <summary>
		/// デフォルトコンストラクタ
		/// </summary>
		ICollisionShape() {};
		/// <summary>
		/// デフォルトデストラクタ
		/// </summary>
		virtual ~ICollisionShape() {};

		/// <summary>
		/// 更新処理
		/// </summary>
		/// <returns>形状</returns>
		virtual void Update() = 0;

		/// <summary>
		/// ポインタをセットする関数
		/// </summary>
		/// <param name="ptr"></param>
		void SetFollowPtr(Object::TransformQuat* ptr) { follow_ = ptr; }
		/// <summary>
		/// 自身の形状を返す純粋仮想関数関数
		/// </summary>
		/// <returns>形状</returns>
		virtual Shape GetShape() = 0;
		/// <summary>
		/// Debug用のImGui
		/// </summary>
		virtual void DebugGUI() = 0;


	public: // ** 各形状との当たり判定関数 ** //

		virtual bool CheckCollision(AABB& c) = 0;
		//virtual bool CheckCollision(OBB& c) = 0;
		virtual bool CheckCollision(Sphere& c) = 0;
		//virtual bool CheckCollision(Capsule& c) = 0;

	protected:
		
		// ヒット時の処理をまとめた関数
		virtual void Hit() {};
	};
};

/// <summary>
/// ICollisionShapeを継承したクラスのみを選択できるテンプレート
/// </summary>
template<class ColliderT>
concept IsICollider = std::is_base_of<LWP::Object::Collider::ICollisionShape, ColliderT>::value;