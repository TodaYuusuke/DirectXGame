#include "Particle.h"

#include "base/ImGuiManager.h"
#include "object/ObjectManager.h"

using namespace LWP;

namespace LWP::Object {
	Particle::Data::Data(const Resource::RigidModel& m) {
		this->m = m;
	}
	Particle::Data::~Data() {
		if (collider) { delete collider; }
	}
	Particle::Data& Particle::Data::operator=(const Data& other) {
		if (this != &other) {
			m = other.m;
			velocity = other.velocity;
			collider = other.collider;
			elapsedTime = other.elapsedTime;
		}
		return *this;
	}

	Particle::Particle() {
		Object::Manager::GetInstance()->SetPtr(this);	// ポインタをセット
	}
	Particle::~Particle() {
		Object::Manager::GetInstance()->DeletePtr(this);	// ポインタを解除
	}

	void Particle::Update(Base::RendererManager* manager) {
		if (!isActive) { return; }
		// 必須の設定だがいい感じに設定できるタイミングがないので無理やりここで設定
		model.isActive = false;

		// イテレータを使用してリストを走査しながら要素を削除
		for (auto it = data_.begin(); it != data_.end(); ) {
			if (UpdateParticle(*it)) {
				it = data_.erase(it); // 要素を削除し、新しいイテレータを取得
			}
			else {
				it++; // 次の要素に進む
			}
		}
		// パーティクル用のデータ登録関数を呼び出す
		//manager->AddParticleData(primitive, data);
		manager;
	}

		// パーティクルを追加
	void Particle::Add(int value) {
		for (int i = 0; i < value; i++) {
			data_.emplace_back(model);
			data_.back().m.isActive = true;
			data_.back().idNumber = IDNumber++;	// 識別番号を付与
			Generate(data_.back());
		}
	}
	void Particle::Add(int value, LWP::Math::Vector3 position) {
		model.worldTF.translation = position;
		Add(value);
	}
		// デバッグ用GUI
	void Particle::DebugGUI() {
		if (ImGui::TreeNode("Model")) {
			model.DebugGUI();
			ImGui::TreePop();
		}
		static int value = 8;
		ImGui::InputInt("Value", &value);
		if (ImGui::Button("Particle Generate")) { Add(value); }
		ImGui::Checkbox("isActive", &isActive);
		ImGui::Text("Count %d", data_.size());
	}
}