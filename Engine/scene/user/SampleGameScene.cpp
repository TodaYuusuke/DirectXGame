#include "SampleGameScene.h"
#include "SampleTitle.h"

using namespace LWP;
using namespace LWP::Resource;
using namespace LWP::Input;
using namespace LWP::Math;
using namespace LWP::Primitive;
using namespace LWP::Utility;

// 初期化
void SampleGameScene::Initialize() {
	// 球
	//sphere = new Sphere();
	sphere.radius = 0.3f;
	sphere.transform.translation.x = 0.0f;
	sphere.transform.translation.z = 3.0f;
	sphere.material.enableLighting = true;
	sphere.material.shininess = 40.0f;

	motion.Add(&sphere.transform.translation, Vector3{ 1.0f,0.0f,0.0f }, 0.0f, 1.0f).
		Add(&sphere.transform.translation, Vector3{ 0.0f,1.0f,0.0f }, 1.0f, 1.0f).
		Add(&sphere.transform.translation, Vector3{ 0.0f,-1.0f,0.0f }, 2.0f, 1.0f);
}
// 更新
void SampleGameScene::Update() {
	// SPACEキーを押すとアニメーション
	if (Keyboard::GetTrigger(DIK_SPACE)) {
		motion.Start();
	}

	// Rキーを押すとシーン再読み込み
	if (Input::Keyboard::GetTrigger(DIK_R)) {
		nextSceneFunction = []() { return new SampleGameScene(); };
	}
	// ENTERキーを押すとシーン切り替え
	if (Input::Keyboard::GetTrigger(DIK_RETURN)) {
		nextSceneFunction = []() { return new SampleTitle(); };
	}
}