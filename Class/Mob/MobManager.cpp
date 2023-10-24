#include "MobManager.h"

using namespace LWP::Math;
using namespace LWP::Utility;

void MobManager::Initialize() {
	target_.Initialize();
	enemies_.clear();
}

void MobManager::Update(MapManager* mapManager) {
	target_.Update();
	mapManager->CheckCollision(&target_);

	int i = 0;
	ImGui::Begin("Mobs");
	for (auto it = enemies_.begin(); it != enemies_.end(); ++it) {
		Enemy* ptr = &(*it);  // イテレータを通じて要素へのポインタを取得
		ptr->Update();
		mapManager->CheckCollision(ptr);
		ImGui::Text((std::to_string(i) + " hp ... %d").c_str(), ptr->hp_);
	}
	ImGui::End();

	// isActiveがfalseになったオブジェクトを削除する
	enemies_.remove_if([](const Enemy& obj) { return !obj.isActive_; });
}

void MobManager::AddEnemy(int addAmount) {
	for (int i = 0; i < addAmount; i++) {
		Vector3 summonPos = Vector3{ 0.9f,0.0f,0.0f } *Matrix4x4::CreateRotateYMatrix(float(GenerateRandamNum<int>(0, 614) / 100.0f));

		enemies_.push_back(Enemy());
		enemies_.back().Initialize();
		enemies_.back().model_->transform.translation = { summonPos };
		enemies_.back().SetTarget(target_.GetTransform());
	}
}