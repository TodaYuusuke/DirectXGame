#pragma once
#include "../ICollider2D.h"
namespace LWP::Object::Collider2D {
	/// <summary>
	/// 2D当たり判定用のRectangleクラス
	/// </summary>
	class Rectangle final
		: public ICollider {
	public: // ** パブリックなメンバ変数 ** //
		// サイズ
		LWP::Math::Vector2 size = { 200.0f,100.0f };
		// 回転量
		float rotation = 0.0f;

	public: // ** メンバ関数 ** //
		// コンストラクタ
		Rectangle();

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
			LWP::Math::Vector2 center;	// 中心座標
			LWP::Math::Vector2 size;	// サイズ
			float rotation;				// 回転量

			// コンストラクタ
			Data(const Rectangle& rect);

			/// <summary>
			/// 分離軸判定用のコーナーを取得する関数
			/// </summary>
			/// <returns></returns>
			std::array<LWP::Math::Vector2, 4> GetCorners();

		};
	};


};