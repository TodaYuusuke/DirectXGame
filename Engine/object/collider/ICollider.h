#pragma once
#include "object/WorldTransform.h"
#include "Mask.h"

#if DEMO
namespace LWP::Base {
	class CommandManager;
}
#endif

namespace LWP::Object::Collider {
	// 前方宣言
	class AABB;
	class OBB;
	class Sphere;

	/// <summary>
	/// 当たり判定用の基底クラス
	/// </summary>
	class ICollider {
	public: // ** パブリックなメンバ関数 ** //
		// デストラクタ
		~ICollider() = default;

		// 追従するトランスフォームのポインタをセットする関数
		void SetFollowTarget(LWP::Object::WorldTransform* ptr) { followPtr_ = ptr; }
		// ヒット時に正常な位置に修正するベクトルを加算
		void AdjustPosition(const LWP::Math::Vector3& fixVector) { followPtr_->translation += fixVector; }

		// ワールド座標を取得
		LWP::Math::Vector3 GetWorldPosition() { return followPtr_->GetWorldPosition(); }
		// ImGui
		void DebugGUI();
#if DEMO
		// ** デバッグ用の描画関数 ** //
		virtual void ShowWireFrame(Base::CommandManager* manager) { manager; }
#endif

	public: // ** 各形状に対する当たり判定 ** //
		virtual bool CheckCollision(AABB* c) = 0;
		virtual bool CheckCollision(OBB* c) = 0;
		virtual bool CheckCollision(Sphere* c) = 0;

	protected: // ** 派生クラス用の関数と変数 ** //
		// 追従するワールドトランスフォーム
		LWP::Object::WorldTransform* followPtr_ = nullptr;
		// 派生クラスで追加のImGuiを実装するとき用の関数
		virtual void DerivedDebugGUI() {/* 基底クラスでは記述なし */}

	public: // ** 共通の変数 ** //
		// マスク処理
		Mask mask;

		// 動くかのフラグ
		bool isMove = true;
		// 前フレーム当たっていたかのフラグ
		bool preHit = false;
		// アクティブ切り替え
		bool isActive = true;
#if DEMO
		// ** デバッグ用の変数 ** //
		bool isShowWireFrame = true;
#endif

	};
};