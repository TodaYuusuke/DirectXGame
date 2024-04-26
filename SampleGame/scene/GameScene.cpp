#include "GameScene.h"
#include "Title.h"

using namespace LWP;
using namespace LWP::Resource;
using namespace LWP::Input;
using namespace LWP::Math;
using namespace LWP::Primitive;
using namespace LWP::Utility;

// 初期化
void GameScene::Initialize() {
	buildings[0].LoadFile("buildings/1Story.obj");
	buildings[1].LoadFile("buildings/1Story_GableRoof.obj");
	buildings[2].LoadFile("buildings/1Story_RoundRoof.obj");
	buildings[3].LoadFile("buildings/1Story_Sign.obj");
	buildings[4].LoadFile("buildings/2Story.obj");
	buildings[5].LoadFile("buildings/2Story_Center.obj");
	for (int i = 0; i < 6; i++) {
		buildings[i].transform.translation.x += i * 2.0f;
		buildings[i].transform.translation.z = 2.0f;
		buildings[i].transform.rotation.y = 3.14;
		buildings[i].material.enableLighting = true;
	}

	skydome.LoadFile("skydome/skydome.obj");
	skydome.transform.scale = { 20.0f,20.0f, 20.0f };
	ground.LoadFile("ground/Ground.gltf");
	ground.transform.scale = { 100.0f,100.0f, 100.0f };

	// プレイヤー初期化
	player.Init(&mainCamera);
}
// 更新
void GameScene::Update() {
	player.Update();
}