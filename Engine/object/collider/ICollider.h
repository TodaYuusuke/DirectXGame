#pragma once
#include "primitive/IPrimitive.h"
#include "utility/Observer.h"
#include "Mask.h"

#include <functional>

#if DEMO
namespace LWP::Base {
	class CommandManager;
}
#endif

namespace LWP::Object::Collider {
	// 識別子
	enum class Shape : int {
		AABB = 0,
		OBB = 1,
		Sphere = 2,
		Count = 3,	// カウント
	};
	// 現在のヒット状況
	enum class OnCollisionState : int {
		None = 0b00,
		Trigger = 0b01,
		Press = 0b11,
		Release = 0b10,
	};

	// 別名定義のためだけに前方宣言させられてる
	class ICollider;
	// ヒット時の関数ポインタの型
	typedef void (*OnColliderFunction)(ICollider* self, ICollider* hit, OnCollisionState state);
	

	/// <summary>
	/// 当たり判定用の基底クラス
	/// </summary>
	class ICollider {
	public: // ** パブリックなメンバ変数 ** //
		// マスク処理
		Mask mask;

		// 動くかのフラグ
		bool isMove = true;
		// アクティブ切り替え
		bool isActive = true;
#if DEMO
		// ** デバッグ用の変数 ** //
		bool isShowWireFrame = true;
		std::string name = "ICollider";	// 固有名詞
#endif


	public: // ** メンバ関数 ** //
		// デストラクタ
		virtual ~ICollider() = default;

		// 更新処理
		void Update();

		// 追従するプリミティブのポインタをセットする関数
		void SetFollowTarget(LWP::Primitive::IPrimitive* ptr) { follow_ = ptr; }
		// ヒット時に正常な位置に修正するベクトルを加算
		void AdjustPosition(const LWP::Math::Vector3& fixVector) { follow_.t->transform += fixVector; }

		// ヒット時のラムダ式をセットする関数
		void SetOnCollisionLambda(const OnColliderFunction& func) { onCollisionLambda = func; }
		// ヒット時に関数を呼び出す関数（※ユーザー呼び出し禁止）
		void ExecuteLambda(ICollider* hitCollision) { onCollisionLambda(this, hitCollision, static_cast<OnCollisionState>((preHit << 1) + hit)); }

		// 自身の形状を返す純粋仮想関数関数
		virtual Shape GetShape() = 0;
		// ワールド座標を取得
		LWP::Math::Vector3 GetWorldPosition() { return follow_.t->transform.GetWorldPosition(); }
		// ImGui
		void DebugGUI();
#if DEMO
		// ** デバッグ用の描画関数 ** //
		virtual void ShowWireFrame() {}
#endif

	protected: // ** 派生クラス用の関数と変数 ** //
		// 追従する形状
		Utility::Observer<LWP::Primitive::IPrimitive*> follow_ = nullptr;

		// ヒット時のリアクション用の関数
		Collider::OnColliderFunction onCollisionLambda = [](ICollider* self, ICollider* hit, OnCollisionState state) { self; hit; state; };
		
		// 更新時に形状を追従するための処理
		virtual void UpdateShape() {/* 基底クラスでは記述なし */ }
		// 派生クラスで追加のImGuiを実装するとき用の関数
		virtual void DerivedDebugGUI() {/* 基底クラスでは記述なし */}

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