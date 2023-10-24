#include "Title.h"
#include "GameScene.h"
#include "../Class/Ease/Easing.h"
#include <time.h>

using namespace LWP;
using namespace LWP::Primitive;
using namespace LWP::Math;
using namespace LWP::Utility;

// 初期化
void Title::Initialize() {
	// 地面を作る
	ground = LWP::Primitive::CreateInstance<Surface>();
	ground->transform.translation.y = -1.0f;
	ground->transform.rotation.x = 1.57f;
	ground->transform.scale = { 10.0f,10.0f, 10.0f };
	ground->commonColor = new Color(0xD87F00FF);
	ground->material.enableLighting = true;
	
	// テクスチャ読み込み
	uvTexture = LWP::Resource::LoadTexture("uvChecker.png");

	// title
	TitleSurface = LWP::Primitive::CreateInstance<Surface>();
	TitleSurface->isActive = true;
	TitleSurface->isUI = true;
	TitleSurface->texture = uvTexture;
	
	TitleSurface->vertices[0].position = {0.0f,0.0f,0.0f};
	TitleSurface->vertices[1].position = {1000.0f,0.0f,0.0f};
	TitleSurface->vertices[2].position = {1000.0f,250.0f,0.0f};
	TitleSurface->vertices[3].position = {0.0f,250.0f,0.0f };

	TitleSurface->transform.translation = { 140.0f ,20.0f ,0.0f };

	// シーン遷移用
	// トランジションフラグをtrueに
	isTransition_ = false;
	isTransitionSceneEnd_ = false;

	for (size_t i = 0; i < 2; i++)
	{
		transitionSurfase_[i] = LWP::Primitive::CreateInstance<Surface>();
		transitionSurfase_[i]->vertices[0].position = { 0.0f,0.0f };
		transitionSurfase_[i]->vertices[1].position = { 1280.0f,0.0f };
		transitionSurfase_[i]->vertices[2].position = { 1280.0f,720.0f };
		transitionSurfase_[i]->vertices[3].position = { 0.0f,720.0f };
		transitionSurfase_[i]->isUI = true;
		transitionSurfase_[i]->isActive = true;
	}

	transitionSurfase_[0]->transform.translation = { -1280.0f,0.0f,0.0f };
	transitionSurfase_[1]->transform.translation = { 1280.0f,0.0f,0.0f };


	// ボタン
	for (int i = 0; i < 2; i++) {
		buttonModel[i] = LWP::Primitive::CreateInstance<Mesh>();
		buttonModel[i] = LWP::Resource::LoadModel("cube/cube.obj");
		buttonModel[i]->transform.scale = {1.0f,0.5f,0.2f};
		buttonModel[i]->transform.rotation = { 0.7f,0.0f,0.0f };
		buttonModel[i]->isActive = true;
	}
	buttonModel[0]->transform.translation = { -1.3f,-0.8f,2.0f };
	buttonModel[1]->transform.translation = { 1.2f,-0.8f,2.0f };

	// ハンマー
	hammerModel = LWP::Primitive::CreateInstance<Mesh>();
	hammerModel = LWP::Resource::LoadModel("hammer/hammer.obj");
	hammerModel->isActive = true;
	hammerModel->transform.translation = { -1.1f,-0.3f,0.65f };
	hammerModel->transform.translation = { 1.2f,-0.3f,0.65f };

	hammerModel->transform.rotation = { 0.6f,-0.1f,0.0f };
	hammerModel->transform.scale = { 0.3f, 0.3f, 0.3f };


	//
	particleModel = LWP::Primitive::CreateInstance<Mesh>();
	particleModel = LWP::Resource::LoadModel("hammer/hammer.obj");
	particleModel->transform.translation;

	particleModel->transform.rotation = { 0.6f,-0.1f,0.0f };
	particleModel->transform.scale = { 0.3f, 0.3f, 0.3f };
	// パーティクル
	//作成
	for (const auto& newParticle : particles) {
		
		particleModel->transform.translation = newParticle.position;
	}
}

// 更新
void Title::Update() {
	UpdateTimer();
	Shake();
	SceneChange();
	SceneTransition();
	moveHammer();

	if (Input::Controller::GetTrigger(DIXBOX_A)) {
		audio->Play();
	}
	if (Input::Controller::GetTrigger(DIXBOX_B)) {
		audio->Stop();
	}

	// ENTERキーを押すとシーン切り替え
	if (Input::Keyboard::GetTrigger(DIK_RETURN)) {
		nextScene_ = new GameScene();
	}

	// Rキーを押すとシーン再読み込み
	if (LWP::Input::GetTrigger(DIK_R)) {
		nextScene_ = new GameScene();
	}

	// SPACEで攻撃
	if (LWP::Input::GetTrigger(DIK_SPACE) && attackCoolTimer < 0) {
		Attack();
		GenerateParticle(hammerModel->transform.translation);
	}

	// パーティクルの更新
	UpdateParticles();

	if (LWP::Input::GetTrigger(DIK_A)) {
			selectPoint = true;
		
	}
	else if (LWP::Input::GetTrigger(DIK_D)) {
			selectPoint = false;
		
	}

	//次のシーンへ移動
	if (selectPoint && LWP::Input::GetTrigger(DIK_SPACE)) {
		sceneChangeTiemFlag = false;		
		ButtonPush();
		shakeMaxPosition = 20;
	}

	//windowを閉じる
	if (selectPoint == 0 && LWP::Input::GetTrigger(DIK_SPACE)) {
		sceneChangeTiemFlag = false;
		ButtonPush();
		shakeMaxPosition = 20;
	}


	//*******************ImGui*******************//
	ImGui::Begin("Primitive");
	TitleSurface->DebugGUI("TitleSurface");
	buttonModel[0]->DebugGUI("button[0]");
	buttonModel[1]->DebugGUI("button[1]");
	transitionSurfase_[0]->DebugGUI("transitionSurface[0]");
	transitionSurfase_[1]->DebugGUI("transitionSurface[1]");
	hammerModel->DebugGUI("hammerModel");
	ImGui::End();

	ImGui::Begin("hammerPos");
	ImGui::Checkbox("isTransitionSceneEnd_", &isTransitionSceneEnd_);
	ImGui::End();

	// カメラ操作
	ImGui::Begin("Camera");
	mainCamera->DebugGUI();
	ImGui::End();
	//********************************************//

}
// 描画
void Title::Draw() {
} 

void Title::UpdateTimer() {
	attackCoolTimer >= 0 ? attackCoolTimer-- : 0;
	attackAni.Progress(&hammerModel->transform);

	if (selectPoint) {
		buttonAni.Progress(&buttonModel[0]->transform);
	}
	else if (selectPoint == 0) {
		buttonAni.Progress(&buttonModel[1]->transform);

	}
}

void Title::SceneChange() {

	if (sceneChangeTiemFlag) {

		sceneChangeTime--;

		if (sceneChangeTime < 60) {
			isTransitionSceneEnd_ = true;

			if (sceneChangeTime < 0) {
				//次のシーンへ
				nextScene_ = new GameScene();

				sceneChangeTiemFlag = false;
				sceneChangeTime = 65;

				return;
			}
		}
	}

}

void Title::Shake() {
	shakeMaxPosition--;

	if (shakeMaxPosition < 1) {
		shakeMaxPosition = 1;
	}

	shakePosition = rand() % shakeMaxPosition - shakeMaxPosition / 2;

	TitleSurface->transform.translation = { 140.0f + shakePosition ,20.0f + shakePosition,0.0f + shakePosition };

}

void Title::Attack() {
	// アニメーション開始
	attackAni.timer = attackAni.GetFullTIme();

	// 攻撃クールタイム
	attackCoolTimer = 30;
}

void Title::AttackAnimation::Progress(LWP::Object::WorldTransform* transform) {
	timer--;
	if (timer < 0) { return; }
	
		// 振り下ろしアニメーション
	if (timer >= swingUp.time) {
		*transform += swingDown.transform / static_cast<float>(swingDown.time);
	}
	else {
		*transform += swingUp.transform / static_cast<float>(swingUp.time);
	}
	
}

void Title::ButtonPush() {
	// アニメーション開始
	buttonAni.timer = buttonAni.GetFullTIme();

}

void Title::Buttonanimation::Progress(LWP::Object::WorldTransform* transform) {
	timer--;
	if (timer < 0) { return; }

	// 振り下ろしアニメーション
	if (timer >= swingUp.time) {
		*transform += swingDown.transform / static_cast<float>(swingDown.time);
	}
	else {
		*transform += swingUp.transform / static_cast<float>(swingUp.time);

	}

}

void Title::SceneTransition() {
	// 別のシーンへの遷移実行時
	if (isTransitionSceneEnd_) {

		// 矩形が画面外に出るまで移動させる
		if (transitionSurfase_[1]->transform.translation.x >= 640.0f) {
			transitionSurfase_[0]->transform.translation.x += 15.0f;
			transitionSurfase_[1]->transform.translation.x -= 15.0f;
		}

	}// 別のシーンからの遷移実行時
	else if (isTransition_) {

		// 矩形が画面外に出るまで移動させる
		if (transitionSurfase_[1]->transform.translation.x < 1280.0f) {
			transitionSurfase_[0]->transform.translation.x -= 15.0f;
			transitionSurfase_[1]->transform.translation.x += 15.0f;
		}
		else {
			isTransition_ = false;
		}

	}else {

		//// フラグに応じてシーンに遷移
		//if (isGoTitle) {
		//	nextScene_ = new Title();
		//}
		//else {
		//	nextScene_ = new GameScene();
		//}
	}
};

void Title::moveHammer() {
	if (selectPoint) {
		if(hammerModel->transform.translation.x >= -1.1f)
		hammerModel->transform.translation.x -= moveSpeed;

	}
	else {
		if(hammerModel->transform.translation.x <= 1.2f)
		hammerModel->transform.translation.x += moveSpeed;

	}
}

void Title::GenerateParticle(LWP::Math::Vector3& hitPosition) {

	particleModel->isActive = true;
	for (int i = 0; i < 5; i++) {
		particle.position = hitPosition;
		particle.velocity = Vector3(static_cast<float>(rand() % 5 - 2), static_cast<float>(rand() % 5 + 5), 0); // ランダムな速度
		particle.gravity = 0.05f; // 重力の影響
		particle.life = 1.0f; // パーティクルの寿命
		particles.push_back(particle);
	}
}

void Title::UpdateParticles() {
	for (auto& newparticle : particles) {
		particle.position.x += newparticle.velocity.x;
		particle.position.y += newparticle.velocity.y;
		particle.velocity.y -= newparticle.gravity;
		particle.life -= 0.01f;
	}

	// パーティクルが寿命を終えたら削除する
	particles.erase(std::remove_if(particles.begin(), particles.end(),
		[](const Particle& p) { return p.life <= 0.0f; }), particles.end());
	if (particles.empty()) {
		particleModel->isActive = false;
	}
}

