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

	/// <summary>
	/// 当たり判定用の基底クラス
	/// </summary>
	class ICollider {
	public: // ** パブリックなメンバ関数 ** //
		// デストラクタ
		virtual ~ICollider() = default;

		// 追従するトランスフォームのポインタをセットする関数
		void SetFollowTarget(LWP::Primitive::IPrimitive* ptr) { follow_ = ptr; }
		// ヒット時に正常な位置に修正するベクトルを加算
		void AdjustPosition(const LWP::Math::Vector3& fixVector) { follow_.t->transform += fixVector; }

		// 自身の形状を返す関数
		virtual Shape GetShape() = 0;
		// ワールド座標を取得
		LWP::Math::Vector3 GetWorldPosition() { return follow_.t->transform.GetWorldPosition(); }
		// ImGui
		void DebugGUI();
#if DEMO
		// ** デバッグ用の描画関数 ** //
		virtual void ShowWireFrame(Base::CommandManager* manager) { manager; }
#endif

	protected: // ** 派生クラス用の関数と変数 ** //
		// 追従する形状
		Utility::Observer<LWP::Primitive::IPrimitive*> follow_ = nullptr;
		// 前フレーム当たっていたかのフラグ
		bool preHit = false;

		// 派生クラスで追加のImGuiを実装するとき用の関数
		virtual void DerivedDebugGUI() {/* 基底クラスでは記述なし */}

	public: // ** 共通の変数 ** //
		// マスク処理
		Mask mask;

		// 動くかのフラグ
		bool isMove = true;
		// アクティブ切り替え
		bool isActive = true;
#if DEMO
		// ** デバッグ用の変数 ** //
		bool isShowWireFrame = true;
		bool hitting = false;
		std::string name = "ICollider";	// 固有名詞
#endif

	public: // ** ヒット時のリアクション用の関数たち ** //

		// ヒットした瞬間に呼び出される関数
		std::function<void(ICollider* self, ICollider* hit)> trigger = [](ICollider*, ICollider*){};
		// ヒットしている間呼び出される関数
		std::function<void(ICollider* self, ICollider* hit)> stay = [](ICollider*, ICollider*) {};
		// 離れた瞬間に呼び出される関数5
		std::function<void(ICollider* self, ICollider* hit)> exit = [](ICollider*, ICollider*) {};
	};
};

/// <summary>
/// IColliderを継承したクラスのみを選択できるテンプレート
/// </summary>
template<class ColliderT>
concept IsICollider = std::is_base_of<LWP::Object::Collider::ICollider, ColliderT>::value;