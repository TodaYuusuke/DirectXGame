#include "SampleGameScene.h"
#include "SampleTitle.h"

using namespace LWP;
using namespace LWP::Primitive;
using namespace LWP::Utility;

// 初期化
void SampleGameScene::Initialize() {
}
// 更新
void SampleGameScene::Update() {
	// Rキーを押すとシーン再読み込み
	if (Input::Keyboard::GetTrigger(DIK_R)) {
		nextScene_ = new SampleGameScene();
	}
	// ENTERキーを押すとシーン切り替え
	if (Input::Keyboard::GetTrigger(DIK_RETURN)) {
		nextScene_ = new SampleTitle();
	}
}