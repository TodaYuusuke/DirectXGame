#include "GameScene.h"

using namespace LWP::Primitive;

// 初期化
void GameScene::Initialize() {
	tri = LWP::Primitive::CreateInstance<Triangle>();

	tri->vertices[0] = { 0.0f,0.2f,0.0f };
	tri->vertices[1] = { 0.2f,0.0f,0.0f };
	tri->vertices[2] = { -0.2f,0.0f,0.0f };

	mapCSV = ("resources/csv/mapData.csv");
	mapChip_ = new MapChip();
	mapPosition_ = { 0.0f,0.0f,0.0f };
	mapChip_->Initialize(mapPosition_);

	mapChip_->LoadMap(mapCSV);
	
}
// 更新
void GameScene::Update() {
	mapChip_->UpdataMapData();
	
}
// 描画
void GameScene::Draw() {
	mapChip_->Draw();
}