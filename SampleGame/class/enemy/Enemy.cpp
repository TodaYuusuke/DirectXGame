#include "Enemy.h"

using namespace LWP;
using namespace LWP::Math;
using namespace LWP::Input;
using namespace LWP::Object;
using namespace LWP::Resource;
using namespace LWP::Utility;
using namespace LWP::Info;

void Enemy::Init(LWP::Utility::CatmullRom* curve, Player* player) {
	// ポインタを保持
	player_ = player;
	curve_ = curve;
	curve_->t = 0.0f;	// tを初期化

	// モデル初期化
	model_.LoadShortPath("Enemy/Zombie_Basic.gltf");
	model_.worldTF.rotation = Quaternion::CreateFromAxisAngle(Vector3::UnitY(), 3.14f);
	anim_.LoadFullPath("resources/model/Enemy/Zombie_Basic.gltf", &model_);
	anim_.Play("Walk", true);	// 歩きアニメーション
	// コライダー初期化
	collision_.SetFollowTarget(&model_.worldTF);
	collision_.mask.SetBelongFrag(MaskLayer::Enemy);
	collision_.mask.SetHitFrag(MaskLayer::Bullet);
	collision_.enterLambda = [this](Collision* c) {
		isDead_ = true;	// いったん被弾したら死亡
		// 弾のコライダーはオフに
		c->isActive = false;
	};
	Collider::AABB& aabb = collision_.SetBroadShape(Collider::AABB());
	aabb.min = { -0.2f,0.0f,-0.45f };
	aabb.max = { 0.2f,1.35f,0.2f };
}
void Enemy::Update() {
	// 死んでいたら早期リターン
	if (isDead_) { return; }

	float& t = curve_->t;
	t += GetDeltaTimeF() * (1.0f / 30.0f);	// 30秒かけて到達するように
	t = std::clamp<float>(t, 0.0f, 1.0f);	// tを増加

	// 敵の地点を更新
	model_.worldTF.translation = curve_->GetPosition();
	// プレイヤーの方向を見る
	//Vector3 from = Vector3{ 0.0f,0.0,1.0f } * model_.worldTF.rotation;	// 現在向いている方向
	//Vector3 to = player_->GetWorldPosition() - model_.worldTF.GetWorldPosition();	// 次に向く方向
	//Quaternion q = model_.worldTF.rotation * Quaternion::DirectionToDirection(from.Normalize(), to.Normalize());
	//model_.worldTF.rotation = Interp::SlerpQuaternion(model_.worldTF.rotation, q, 0.1f).Normalize();
	

	//float nextT = t + 0.1f;	// カメラの向く方向用のt
	//if (nextT > 1.0f) { nextT = 1.0f; }

	//// 向く方向の座標をセット
	//Vector3 viewDirectionPos = (*curve_)->GetPosition(nextT);
	//if (nextT >= 1.0f) {	// もしnextTがcatmullRomの外側ならば、Z+方向を向く
	//	viewDirectionPos.z -= 1.0f;
	//}

	//// 方向をセット
	//Vector3 from = Vector3{ 0.0f,0.0,1.0f } * model_.worldTF.rotation;	// 現在向いている方向
	//Vector3 to = viewDirectionPos - model_.worldTF.translation;	// 次に向く方向
	//Quaternion q = model_.worldTF.rotation * Quaternion::DirectionToDirection(from.Normalize(), to.Normalize());
	////q.y = 0.0f;	// Yを固定
	//model_.worldTF.rotation = Interp::SlerpQuaternion(model_.worldTF.rotation, q, 0.1f).Normalize();
}

void Enemy::DebugGUI() {
	if (ImGui::TreeNode("Model")) {
		model_.DebugGUI();
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Collision")) {
		collision_.DebugGUI();
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Animation")) {
		anim_.DebugGUI();
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Curve")) {
		curve_->DebugGUI();
		ImGui::TreePop();
	}
}