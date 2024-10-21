#pragma once
#include <Adapter.h>

/// <summary>
/// ハンドガン
/// </summary>
class Pistol final {
public: // ** メンバ関数 ** //
	/// <summary>
	/// コンストラクタ
	/// </summary>
	Pistol();

	/// <summary>
	/// 初期化処理
	/// </summary>
	void Init(LWP::Object::TransformQuat* wtf);
	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

	/// <summary>
	/// 射撃
	/// </summary>
	void Shot(LWP::Math::Vector3 pos, LWP::Math::Vector3 dir);
	/// <summary>
	/// リロード
	/// </summary>
	void Reload();

	/// <summary>
	/// Debug用のImGui
	/// </summary>
	void DebugGUI();

private: // ** メンバ定数 ** //
	// 最大装填数
	int kMaxAmmo_ = 10;

private: // ** メンバ変数 ** //
	// モデル
	LWP::Resource::SkinningModel model_;
	// アニメーション
	//LWP::Resource::Animation anim_;
	// コライダー
	LWP::Object::Collision collision_;
	LWP::Object::Collider::Capsule& capsule_;

	// 装填数
	int ammo_ = 10;
};