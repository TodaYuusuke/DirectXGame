#include "SpriteParticle.h"

#include "base/ImGuiManager.h"
#include "object/ObjectManager.h"

using namespace LWP;

namespace LWP::Object {
	SpriteParticle::Data::Data(const Primitive::SequenceSprite& s) {
		this->s = s;
	}
	SpriteParticle::Data::~Data() {
		if (collision) { delete collision; }
	}
	SpriteParticle::Data& SpriteParticle::Data::operator=(const Data& other) {
		if (this != &other) {
			s = other.s;
			velocity = other.velocity;
			collision = other.collision;
			elapsedTime = other.elapsedTime;
		}
		return *this;
	}

	SpriteParticle::SpriteParticle() {
		Object::Manager::GetInstance()->SetPtr(this);	// ポインタをセット
		Init();
	}
	SpriteParticle::~SpriteParticle() {
		Object::Manager::GetInstance()->DeletePtr(this);	// ポインタを解除
	}

	void SpriteParticle::Update() {
		if (!isActive) { return; }
		// 必須の設定だがいい感じに設定できるタイミングがないので無理やりここで設定
		sprite.isActive = false;

		// イテレータを使用してリストを走査しながら要素を削除
		for (auto it = data_.begin(); it != data_.end(); ) {
			if (UpdateParticle(*it)) {
				it = data_.erase(it); // 要素を削除し、新しいイテレータを取得
			}
			else {
				it++; // 次の要素に進む
			}
		}
	}

		// パーティクルを追加
	void SpriteParticle::Add(int value) {
		for (int i = 0; i < value; i++) {
			data_.emplace_back(sprite);
			data_.back().s.isActive = true;
			data_.back().idNumber = IDNumber++;	// 識別番号を付与
			Generate(data_.back());
		}
	}
	void SpriteParticle::Add(int value, LWP::Math::Vector3 position) {
		sprite.worldTF.translation = position;
		Add(value);
	}
		// デバッグ用GUI
	void SpriteParticle::DebugGUI() {
		if (ImGui::TreeNode("Sprite")) {
			sprite.DebugGUI();
			ImGui::TreePop();
		}
		static int value = 8;
		ImGui::InputInt("Value", &value);
		if (ImGui::Button("Particle Generate")) { Add(value); }
		ImGui::Checkbox("isActive", &isActive);
		ImGui::Text("Count %d", data_.size());
	}
}