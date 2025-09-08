#pragma once
#include "object/TransformQuat.h"
#include "math/Rectangle.h"
#include "primitive/2d/planeInterface/Sprite.h"


namespace LWP::Object::Collider2D {
	// 識別子
	enum class Shape : int {
		//Point,	// 点
		//Line, // 線
		Circle, // 円
		Rectangle, // 矩形
		//Polygon, // 多角形
		Count	// カウント
	};
	// 識別子のstring
	static std::string kShapeString[] = {
		//"Point",
		//"Line",
		"Circle",
		"Rectangle",
		//"Polygon",
		"Count"
	};

	// 前方宣言
	//class Point;
	//class Line;
	class Circle;
	class Rectangle;
	//class Polygon;

	/// <summary>
	/// 2D当たり判定の形状用の基底クラス
	/// </summary>
	class ICollider {
	public: // ** パブリックなメンバ変数 ** //

		// アクティブ切り替え
		bool isActive = true;
		// モートン序列番号
		int mortonNumber = -1;

	protected: // ** プライベートなメンバ変数 ** //

		// Colliderのトランスフォームのポインタ
		Object::TransformQuat* tf_ = nullptr;
#if DEMO
		// デバッグ用に表示するスプライト
		LWP::Primitive::NormalSprite sprite_;
#endif

	public: // ** メンバ関数 ** //

		/// <summary>
		/// デフォルトコンストラクタ
		/// </summary>
		ICollider() {};
		/// <summary>
		/// デストラクタ
		/// </summary>
		virtual ~ICollider() {};

		/// <summary>
		/// 更新処理
		/// </summary>
		/// <returns>形状</returns>
		virtual void Update() = 0;

		/// <summary>
		/// 4分木空間分割用の、自分を囲む最小の矩形.を返す関数
		/// </summary>
		virtual Math::Rectangle2D GetBoundingRect() = 0;
		/// <summary>
		/// ポインタをセットする関数
		/// </summary>
		/// <param name="ptr"></param>
		void SetFollowPtr(Object::TransformQuat* ptr);
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

		//virtual bool CheckCollision(Point& c, Math::Vector3* resVec) = 0;
		//virtual bool CheckCollision(Line& c, Math::Vector3* resVec) = 0;
		virtual bool CheckCollision(Circle& c, Math::Vector3* resVec) = 0;
		virtual bool CheckCollision(Rectangle& c, Math::Vector3* resVec) = 0;
		//virtual bool CheckCollision(Polygon& c, Math::Vector3* resVec) = 0;

		// ヒット時の処理をまとめた関数
		virtual void Hit() = 0;
	};
};

/// <summary>
/// ICollisionShapeを継承したクラスのみを選択できるテンプレート
/// </summary>
template<class Collider2DT>
concept IsICollider2D = std::is_base_of<LWP::Object::Collider2D::ICollider, Collider2DT>::value;