#include "UI.h"

using namespace LWP::Utility;
using namespace LWP::Primitive;

UI& UI::AddSprite(LWP::Resource::Texture texture, LWP::Math::Vector2 position, LWP::Math::Vector2 anchor) {
	sprites_.push_back(Sprite());
	sprites_.back().worldTF.translation.x = position.x;
	sprites_.back().worldTF.translation.y = position.y;
	sprites_.back().anchorPoint = anchor;
	sprites_.back().material.texture.t = texture;
	sprites_.back().isUI = true;
	return *this;
}

// 全部のisActiveを変更する関数
void UI::SetIsActive(bool b) {
	for (int i = 0; i < sprites_.size(); i++) {
		sprites_[i].isActive = b;
	}
}