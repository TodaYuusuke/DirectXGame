#pragma once
#include "object/TransformQuat.h"

// 前方宣言
namespace LWP::Resource {
	class RigidModel;
	class SkinningModel;
}

namespace LWP::Object::Collider {
	// 識別子
	enum class Shape : int {
		Point,
		AABB,
		OBB,
		Sphere,
		Capsule,
		Mesh,
		Terrain,
		Count	// カウント
	};
	// 識別子のstring
	static std::string kShapeString[] = {
		"Point",
		"AABB",
		"OBB",
		"Sphere",
		"Capsule",
		"Mesh",
		"Terrain",
		"Count"
	};

	// 前方宣言
	class Point;
	class AABB;
	//class OBB;
	class Sphere;
	class Capsule;
	class Mesh;
	class Terrain;

	/// <summary>
	/// 当たり判定の形状用の基底クラス
	/// </summary>
	class ICollider {
	public: // ** パブリックなメンバ変数 ** //

		// アクティブ切り替え
		bool isActive = true;
		// モートン序列番号
		int mortonNumber = -1;
#if DEMO
		// ** デバッグ用の変数 ** //
		bool isShowWireFrame = true;
#endif

	protected: // ** プライベートなメンバ変数 ** //

		// Colliderのトランスフォームのポインタ
		Object::TransformQuat* tf_ = nullptr;

	public: // ** メンバ関数 ** //

		/// <summary>
		/// デフォルトコンストラクタ
		/// </summary>
		ICollider() {};
		/// <summary>
		/// デフォルトデストラクタ
		/// </summary>
		virtual ~ICollider() {};

		/// <summary>
		/// 更新処理
		/// </summary>
		/// <returns>形状</returns>
		virtual void Update() = 0;

		/// <summary>
		/// 8分木空間分割用の、自分を囲む最小のAABBを返す関数
		/// </summary>
		/// <param name="min"></param>
		/// <param name="max"></param>
		virtual void GetBoundingAABB(LWP::Math::Vector3* minPtr, LWP::Math::Vector3* maxPtr) = 0;
		/// <summary>
		/// ポインタをセットする関数
		/// </summary>
		/// <param name="ptr"></param>
		void SetFollowPtr(Object::TransformQuat* ptr) { tf_ = ptr; }
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

		virtual bool CheckCollision(Point& c, Math::Vector3* resVec) = 0;
		virtual bool CheckCollision(AABB& c, Math::Vector3* resVec) = 0;
		virtual bool CheckCollision(Sphere& c, Math::Vector3* resVec) = 0;
		virtual bool CheckCollision(Capsule& c, Math::Vector3* resVec) = 0;

		// ヒット時の処理をまとめた関数
		virtual void Hit() {};
	};
};

/// <summary>
/// ICollisionShapeを継承したクラスのみを選択できるテンプレート
/// </summary>
template<class ColliderT>
concept IsICollider = std::is_base_of<LWP::Object::Collider::ICollider, ColliderT>::value;