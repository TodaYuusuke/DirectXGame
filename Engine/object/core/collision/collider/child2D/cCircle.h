#pragma once
#include "../ICollider2D.h"

namespace LWP::Object::Collider2D {
	/// <summary>
	/// 2D当たり判定用のCircleクラス
	/// </summary>
	class Circle final
		: public ICollider {
	public: // ** パブリックなメンバ変数 ** //
		// 中心点はworldTFで管理
		
		// 半径
		float radius = 50.0f;

	public: // ** メンバ関数 ** //
		// コンストラクタ
		Circle();
		Circle(const float& radius);
		// デストラクタ
		~Circle();

		// 固有の更新処理
		void Update() override;
		// 自分を囲む最小の矩形を返す関数
		virtual Math::Rectangle2D GetBoundingRect() override;

		// 形状を返す
		Shape GetShape() override { return Shape::Rectangle; }
		// ImGuiの派生クラス
		void DebugGUI() override;


	public: // ** 各形状との当たり判定関数 ** //

		//bool CheckCollision(Point& c, Math::Vector3* resVec) override;
		//bool CheckCollision(Line& c, Math::Vector3* resVec) override;
		bool CheckCollision(Circle& c, Math::Vector3* resVec) override;
		bool CheckCollision(Rectangle& c, Math::Vector3* resVec) override;
		//bool CheckCollision(Polygon& c, Math::Vector3* resVec) override;

		// ヒット時の処理をまとめた関数
		void Hit() override;

		// 当たり判定計算に適したデータ構造体
		struct Data {
			// 中心点
			LWP::Math::Vector2 center;
			// 半径
			float radius;

			// コンストラクタ
			Data(const Circle& circle);
		};
	};


};