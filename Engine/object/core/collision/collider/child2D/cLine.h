#pragma once
#include "../ICollider2D.h"

namespace LWP::Object::Collider2D {
	/// <summary>
	/// 当たり判定用のAABBクラス
	/// </summary>
	class Line final
		: public ICollider {
	public: // ** パブリックなメンバ変数 ** //
		// 始点
		LWP::Math::Vector2 start;
		// 終点
		LWP::Math::Vector2 end;


	public: // ** メンバ関数 ** //
		// コンストラクタ
		Line();
		Line(const LWP::Math::Vector2& start, const LWP::Math::Vector2& end);

		// 固有の更新処理
		void Update() override;
		// 自分を囲む最小の矩形を返す関数
		virtual Math::Rectangle2D GetBoundingRect() override;

		// 形状を返す
		Shape GetShape() override { return Shape::Line; }
		// ImGuiの派生クラス
		void DebugGUI() override;


	public: // ** 各形状との当たり判定関数 ** //

		bool CheckCollision(Point& c, Math::Vector3* resVec) override;
		bool CheckCollision(Line& c, Math::Vector3* resVec) override;
		bool CheckCollision(Circle& c, Math::Vector3* resVec) override;
		bool CheckCollision(Rectangle& c, Math::Vector3* resVec) override;
		bool CheckCollision(Polygon& c, Math::Vector3* resVec) override;

		// 当たり判定計算に適したデータ構造体
		struct Data {
			// 始点
			LWP::Math::Vector2 start;
			// 終点
			LWP::Math::Vector2 end;

			// コンストラクタ
			Data(const Line& aabb);
		};
	};


};