#include "Title.h"
#include "Tutorial.h"

using namespace LWP::Primitive;
using namespace LWP::Resource;
using namespace LWP::Input;

// 初期化
void Title::Initialize() {
	// スプライト生成
	backGround_ = CreateInstance<Sprite>();
	backGround_->texture = LoadTexture("titleScene.png");
	backGround_->isUI = true;

	// サウンドデータ読み込み
	//gameStartSE_ = LoadAudio("title/GameStart.wav");
	//gameStartSE_->SetVolume(0.5f);
}
// 更新
void Title::Update() {
	// Xトリガーを押していたら >>> 次のシーンへ
	if (Pad::GetTrigger(0, XBOX_X) || Keyboard::GetTrigger(DIK_SPACE)) {
		//gameStartSE_->Play();
		nextScene_ = new Tutorial();
	}
}