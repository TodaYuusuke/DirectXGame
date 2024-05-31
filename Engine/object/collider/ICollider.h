#pragma once

#include "resources/model/RigidModel.h"
#include "utility/observers/Observer2.h"
#include "Mask.h"

#include <functional>

namespace LWP::Object::Collider {
	// 識別子
	enum class Shape : int {
		AABB = 0,
		OBB = 1,
		Sphere = 2,
		Capsule = 3,
		Count	// カウント
	};
	// 現在のヒット状況
	enum class OnCollisionState : int {
		NoHit = 0b00,
		Enter = 0b01,
		Stay = 0b11,
		Exit = 0b10,
	};
	// 別名定義のためだけに前方宣言させられてる
	class ICollider;
	// コールバック用構造体
	struct HitData {
		// 自身の当たり判定クラス
		ICollider* self;
		// 当たった対象の当たり判定クラス
		ICollider* hit;
	};

	// ヒット時の関数ポインタの型
	typedef std::function<void(HitData hitData)> OnColliderFunction;

	/// <summary>
	/// 当たり判定用の基底クラス
	/// </summary>
	class ICollider {
	public: // ** パブリックなメンバ変数 ** //
		// マスク処理
		Mask mask;

		// 固有名詞
		std::string name = "ICollider";
		// 動くかのフラグ
		bool isMove = true;
		// アクティブ切り替え
		bool isActive = true;
#if DEMO
		// ** デバッグ用の変数 ** //
		bool isShowWireFrame = true;
#endif

		// - ヒット時のリアクション用の関数 - //

		// ヒットしていないとき
		OnColliderFunction noHitLambda = [](HitData data) { data; };
		// ヒットした瞬間のとき
		OnColliderFunction enterLambda = [](HitData data) { data; };
		// ヒットし続けているとき
		OnColliderFunction stayLambda = [](HitData data) { data; };
		// ヒットが離れたとき
		OnColliderFunction exitLambda = [](HitData data) { data; };


	public: // ** メンバ関数 ** //
		/// <summary>
		/// デフォルトコンストラクタ
		/// </summary>
		ICollider();
		/// <summary>
		/// デフォルトデストラクタ
		/// </summary>
		virtual ~ICollider();

		// 更新処理
		virtual void Update();

		// 追従するプリミティブのポインタをセットする関数
		void SetFollowTarget(LWP::Resource::RigidModel* ptr) { followModel_ = ptr; }
		// ヒット時に正常な位置に修正するベクトルを加算
		void AdjustPosition(const LWP::Math::Vector3& fixVector) { followModel_.t->worldTF += fixVector; }

		// ヒット時に関数を呼び出す関数（※ユーザー呼び出し禁止）
		void ExecuteLambda(ICollider* hitCollision);

		// 自身の形状を返す純粋仮想関数関数
		virtual Shape GetShape() = 0;
		// ワールド座標を取得
		LWP::Math::Vector3 GetWorldPosition() { return followModel_.t->worldTF.GetWorldPosition(); }
		// ImGui
		virtual void DebugGUI();


	protected: // ** 派生クラス用の関数と変数 ** //
		// 追従するモデル
		Utility::ObserverStruct<Resource::RigidModel*, Resource::RigidModelStruct> followModel_ = nullptr;

		// 前フレーム当たっていたかのフラグ
		bool hit = false;
		bool preHit = false;
	public: // ** アクセッサ ** //
		// ヒット判定のゲッター
		bool GetHit() { return hit; }
		// ヒット判定のセッター
		void SetHit(bool b) { hit = b; }
	};
};

/// <summary>
/// IColliderを継承したクラスのみを選択できるテンプレート
/// </summary>
template<class ColliderT>
concept IsICollider = std::is_base_of<LWP::Object::Collider::ICollider, ColliderT>::value;