#pragma once
#include "ICollider.h"

#if DEMO
#include "resources/model/standard/Cube.h"
#endif

namespace LWP::Object::Collider {
	/// <summary>
	/// 当たり判定用のAABBクラス
	/// </summary>
	class AABB 
		: public ICollider {
	public: // ** パブリックなメンバ変数 ** //
		// 最小
		LWP::Math::Vector3 min = { -0.5f,-0.5f,-0.5f };
		// 最大
		LWP::Math::Vector3 max = { 0.5f,0.5f,0.5f };

#if DEMO
	protected:
		// デバッグ用モデル
		LWP::Resource::Cube cube;
#endif
	public: // ** メンバ関数 ** //
		// コンストラクタ
		AABB();
		AABB(const LWP::Math::Vector3& min, const LWP::Math::Vector3& max);

		// 固有の更新処理
		void Update() override;
		// ImGuiの派生クラス
		void DebugGUI() override;

		// 場所を指定して生成する関数
		virtual void Create(const LWP::Math::Vector3& position);
		virtual void Create(const LWP::Math::Vector3& position, const LWP::Math::Vector3& size);
		// 形状から包み込む最小のAABBを生成する関数
		virtual void Create(LWP::Resource::RigidModel* model);
		// 形状を返す
		Shape GetShape() override { return Shape::AABB; }

		// Observer用（==）
		bool operator==(AABB& other) {
			return { 
				worldTF.GetChanged() &&
				min == other.min &&
				max == other.max
			};
		}


	protected: // ** プライベートな関数 ** //
	};


	// データ構造体
	struct AABB_Data {
		// 最小
		LWP::Math::Vector3 min;
		// 最大
		LWP::Math::Vector3 max;
		// 中心の座標
		LWP::Math::Vector3 center;

		// コンストラクタ
		AABB_Data(const AABB& aabb) {
			min = aabb.min;
			max = aabb.max;
			center = (min + max) / 2.0f;
		}
	};
};