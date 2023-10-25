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
		
	// テクスチャ読み込み
	uvTexture = LWP::Resource::LoadTexture("uvChecker.png");

	skydome_.Initialze();

	///////ルール
	rule = LWP::Primitive::CreateInstance<Surface>();
	rule->texture = LWP::Resource::LoadTexture("rule.png");
	rule->isActive = false;
	rule->transform.translation = { 0.0f,0.0f,0.0f };
	rule->transform.rotation = { 0.0f,0.0f,0.0f };
	rule->transform.scale = { 10.0f,6.0f,1.0f };

	// title
	for (int i = 0; i < 5; i++) {
		tambourineModel[i] = LWP::Primitive::CreateInstance<Mesh>();
		tambourineModel[i] = LWP::Resource::LoadModel("tan/tan.obj");
		tambourineModel[i]->isActive = true;
		tambourineModel[i]->transform.rotation = { -1.5f,0.0f,0.0f };

	}

	tambourineModel[0]->transform.translation = { -2.8f,1.45f,1.0f };
	tambourineModel[1]->transform.translation = { -1.3f,1.45f,0.65f };
	tambourineModel[2]->transform.translation = { 0.0f,1.45f,0.65f };
	tambourineModel[3]->transform.translation = { 1.3f,1.45f,0.65f };
	tambourineModel[4]->transform.translation = { 2.8f,1.45f,1.0f };

	tambourineModel[0]->transform.rotation = { -1.5f,-0.5f,0.0f };
	tambourineModel[1]->transform.rotation = { -1.5f,-0.3f,0.0f };
	tambourineModel[2]->transform.rotation = { -1.5f,0.0f,0.0f };
	tambourineModel[3]->transform.rotation = { -1.5f,0.3f,0.0f };
	tambourineModel[4]->transform.rotation = { -1.5f,0.5f,0.0f };

	tambourineModel[0]->transform.scale = { 1.5f,1.5f,1.5f };
	tambourineModel[1]->transform.scale = { 1.2f,1.2f,1.2f };
	tambourineModel[2]->transform.scale = { 1.5f,1.5f,1.5f };
	tambourineModel[3]->transform.scale = { 1.2f,1.2f,1.2f };
	tambourineModel[4]->transform.scale = { 1.5f,1.5f,1.5f };

	//titleText
	textDO = LWP::Primitive::CreateInstance<Surface>();
	textDO->texture = LWP::Resource::LoadTexture("UI/do.png");
	for (int i = 0; i < 2; i++) {
		textN[i] = LWP::Primitive::CreateInstance<Surface>();
		textN[i]->texture = LWP::Resource::LoadTexture("UI/n.png");
	}

	textBA = LWP::Primitive::CreateInstance<Surface>();
	textBA->texture = LWP::Resource::LoadTexture("UI/ba.png");
	textRI = LWP::Primitive::CreateInstance<Surface>();
	textRI->texture = LWP::Resource::LoadTexture("UI/ri.png");

	textDO->transform.translation = { -2.6f,1.45f,0.65f };
	textN[0]->transform.translation = { -1.3f,1.45f,0.65f };
	textBA->transform.translation = { 0.0f,1.45f,0.65f };
	textRI->transform.translation = { 1.3f,1.45f,0.65f };
	textN[1]->transform.translation = { 2.6f,1.45f,0.65f };

	textDO->transform.rotation = { 0.0f,-0.5f,0.0f };
	textN[0]->transform.rotation = { 0.0f, -0.3f, 0.0f };
	textBA->transform.rotation = { 0.0f,0.0f,0.0f };
	textRI->transform.rotation = { 0.0f,0.3f,0.0f };
	textN[1]->transform.rotation  = { 0.0f, 0.5f, 0.0f };

	textDO->transform.scale = { 1.3f,1.3f,1.0f };
	textBA->transform.scale = { 1.3f,1.3f,1.0f };
	textN[1]->transform.scale = { 1.3f,1.3f,1.0f };

	// ハンマー
	hammerModel = LWP::Primitive::CreateInstance<Mesh>();
	hammerModel = LWP::Resource::LoadModel("hammer/hammer.obj");
	hammerModel->isActive = true;
	hammerModel->transform.translation = { 0.7f,-0.9f,-2.7f };

	hammerModel->transform.rotation = { -2.0f,0.6f,0.0f };
	hammerModel->transform.scale = { 1.2f, 1.2f, 1.2f };


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
		buttonModel[i] = LWP::Resource::LoadModel("switch/switch.obj");
		buttonModel[i]->transform.scale = {0.4f,0.4f,0.4f};
		buttonModel[i]->isActive = true;
		/*buttonModel[i]->material.enableLighting = true;*/

		buttonFoundationModel[i] = LWP::Primitive::CreateInstance<Mesh>();
		buttonFoundationModel[i] = LWP::Resource::LoadModel("switch/switchFoundation.obj");
		buttonFoundationModel[i]->transform.scale = { 0.7f,0.7f,0.7f };
		
		buttonFoundationModel[i]->isActive = true;
		buttonFoundationModel[i]->material.enableLighting = true;
	}
	buttonModel[0]->transform.translation = { -1.8f,-1.3f,1.2f };
	buttonModel[1]->transform.translation = { 1.77f,-1.3f,1.2f };

	buttonModel[0]->transform.rotation = { -1.0f,-0.4f,0.0f };
	buttonModel[1]->transform.rotation = { 1.0f,3.5f,0.0f };

	buttonFoundationModel[0]->transform.translation = { -2.0f,-1.7f,2.0f };
	buttonFoundationModel[1]->transform.translation = { 2.0f,-1.7f,2.0f };

	buttonFoundationModel[0]->transform.rotation = { 0.8f,2.8f,0.0f };
	buttonFoundationModel[1]->transform.rotation = { 0.8f,3.6f,0.0f };
	//下のUI
	startUI = LWP::Primitive::CreateInstance<Surface>();
	startUI->texture = LWP::Resource::LoadTexture("UI/start.png");
	startUI->isActive = true;
	startUI->transform.translation = { -1.6f,-2.0f,0.0f };
	startUI->transform.rotation = { 0.0f,0.0f,0.0f };
	startUI->transform.scale = { 1.6f,1.0f,0.0f };

	ruleUI = LWP::Primitive::CreateInstance<Surface>();
	ruleUI->texture = LWP::Resource::LoadTexture("UI/rule.png");
	ruleUI->isActive = true;
	ruleUI->transform.translation = { 1.6f,-2.0f,0.0f };
	ruleUI->transform.rotation = { 0.0f,0.0f,0.0f };
	ruleUI->transform.scale = { 1.6f,1.0f,0.0f };
}

// 更新
void Title::Update() {
	UpdateTimer();
	Shake();
	SceneChange();
	SceneTransition();
	moveHammer();
	TitleUI();
	Controller();

	// ENTERキーを押すとシーン切り替え
	if (Input::Keyboard::GetTrigger(DIK_RETURN)) {
		nextScene_ = new GameScene();
	}

	if (Input::Keyboard::GetTrigger(DIK_A)) {
		selectPoint = true;
		
	}
	else if (Input::Keyboard::GetTrigger(DIK_D)) {
		selectPoint = false;
		
	}

	//次のシーンへ移動
	if (selectPoint && Input::Keyboard::GetTrigger(DIK_SPACE) && attackCoolTimer < 0) {
		sceneChangeTiemFlag = true;		
		ButtonPush();
		shakeMaxPosition = 20;
		Attack();
	}

	//windowを閉じる
	if (selectPoint == 0 && Input::Keyboard::GetTrigger(DIK_SPACE) && attackCoolTimer < 0) {
		sceneChangeTiemFlag = false;
		ButtonPush();
		shakeMaxPosition = 20;
		Attack();
	}

	//*******************ImGui*******************//
	ImGui::Begin("Primitive");
	TitleSurface->DebugGUI("TitleSurface");
	buttonModel[0]->DebugGUI("button[0]");
	buttonModel[1]->DebugGUI("button[1]");
	transitionSurfase_[0]->DebugGUI("transitionSurface[0]");
	transitionSurfase_[1]->DebugGUI("transitionSurface[1]");
	hammerModel->DebugGUI("hammerModel");
	tambourineModel[0]->DebugGUI("tambourine[0]");
	tambourineModel[1]->DebugGUI("tambourine[1]");
	tambourineModel[2]->DebugGUI("tambourine[2]");
	tambourineModel[3]->DebugGUI("tambourine[3]");
	tambourineModel[4]->DebugGUI("tambourine[4]");
	textBA->DebugGUI("textBA");
	buttonFoundationModel[0]->DebugGUI("buttonFoundation[0]");
	buttonFoundationModel[1]->DebugGUI("buttonFoundation[1]");
	rule->DebugGUI("rule");
	startUI->DebugGUI("startUI");
	ruleUI->DebugGUI("ruleUI");

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
		buttonAni.Progress(&buttonFoundationModel[0]->transform);
	}
	else if (selectPoint == 0) {
		buttonAni.Progress(&buttonModel[1]->transform);
		buttonAni.Progress(&buttonFoundationModel[1]->transform);

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

	float randValue = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
	float scaledRandValue = randValue * shakeMaxPosition - (shakeMaxPosition / 2);

	for (int i = 0; i < 5; i++) {
		tambourineModel[i]->transform.translation =
		{
			tambourineModel[i]->transform.translation.x + scaledRandValue,
			tambourineModel[i]->transform.translation.y + scaledRandValue,
			tambourineModel[i]->transform.translation.z + scaledRandValue
		};
	}
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
		if (hammerModel->transform.translation.x >= -0.7f) {
			hammerModel->transform.translation.x -= moveSpeed;
		}

		if (hammerModel->transform.rotation.y >= -0.5f) {
			hammerModel->transform.rotation.y -= moveSpeed;
		}

	}
	else {
		if (hammerModel->transform.translation.x <= 0.4f) {
			hammerModel->transform.translation.x += moveSpeed;
		}
		if (hammerModel->transform.rotation.y <= 0.7f) {
			hammerModel->transform.rotation.y += moveSpeed;
		}
		
	}
}

void Title::GenerateParticle(LWP::Math::Vector3& hitPosition) {

	particleModel->isActive = false;
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

void Title::TitleUI() {
	tambourineModel[0]->transform.translation = textDO->transform.translation;
	tambourineModel[1]->transform.translation = textN[0]->transform.translation;
	tambourineModel[2]->transform.translation = textBA->transform.translation;
	tambourineModel[3]->transform.translation = textRI->transform.translation;
	tambourineModel[4]->transform.translation = textN[1]->transform.translation;


}

void Title::Controller(){
	if (rule->isActive == false) {
		hammerModel->isActive = true;
		ruleUI->isActive = true;
		startUI->isActive = true;

		if (Input::Controller::GetLStick().x >= 1.0f) {
			selectPoint = false;

		}
		else if (Input::Controller::GetLStick().x <= -1.0f) {
			selectPoint = true;

		}

		//次のシーンへ移動
		if (selectPoint && Input::Controller::GetTrigger(DIXBOX_A) && attackCoolTimer < 0) {
			sceneChangeTiemFlag = true;
			ButtonPush();
			shakeMaxPosition = 20;
			Attack();
		}

		//windowを閉じる
		if (selectPoint == 0 && Input::Controller::GetTrigger(DIXBOX_A) && attackCoolTimer < 0) {
			rule->isActive = true;
			ButtonPush();
			shakeMaxPosition = 20;
			Attack();
		}
	}
	else {
		hammerModel->isActive = false;
		ruleUI->isActive = false;
		startUI->isActive = false;
	}

	if (Input::Controller::GetTrigger(DIXBOX_B)) {
		rule->isActive = false;
	}

}