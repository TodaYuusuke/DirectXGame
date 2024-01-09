#include "Tutorial.h"
#include "GamePlay.h"

using namespace LWP::Primitive;
using namespace LWP::Resource;
using namespace LWP::Input;

// 初期化
void Tutorial::Initialize() {
	// スプライト生成
	backGround_ = CreateInstance<Sprite>();
	backGround_->texture = LoadTexture("tutorialScene.png");
	backGround_->isUI = true;

	// サウンドデータ読み込み
	//gameStartSE_ = LoadAudio("title/GameStart.wav");
	//gameStartSE_->SetVolume(0.5f);
}
// 更新
void Tutorial::Update() {
	// Xトリガーを押していたら >>> 次のシーンへ
	if (Pad::GetTrigger(0, XBOX_X) || Keyboard::GetTrigger(DIK_SPACE)) {
		//gameStartSE_->Play();
		nextScene_ = new GamePlay();
	}
}