#include "Particle.h"
#include "../../../Adapter/Adapter.h"

using namespace LWP::Object;
using namespace LWP::Math;
using namespace LWP::Info;
using namespace LWP::Resource;

// 初期化
void Particle::Initialize() {}
// 更新
void Particle::Update(Base::CommandManager* manager) {
	if (!isActive || !primitive) { return; }
	// 必須の設定だがいい感じに設定できるタイミングがないので無理やりここで設定
	primitive->isActive = false;
	primitive->isActive = false;

	// 関数ポインタがあるなら実行
	if (func) {
		elapsedFrame++;
		for (int i = 0; i < data.size(); i++) {
			if (func(&data[i], elapsedFrame)) {
				data.erase(data.begin() + i);
				i--;
			}
		}
	}
	// パーティクル用のデータ登録関数を呼び出す
	manager->SetParticleData(primitive, data);
}

void Particle::DebugGUI() {
	primitive->DebugGUI();
	ImGui::Checkbox("isActive", &isActive);
}