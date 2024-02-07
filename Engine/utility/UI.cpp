#include "UI.h"

using namespace LWP::Utility;
using namespace LWP::Primitive;

UI& UI::AddSprite(LWP::Resource::Texture* texture, LWP::Math::Vector2 position, LWP::Math::Vector2 anchor) {
	Sprite* newSprite = CreateInstance<Sprite>();
	newSprite->transform.translation.x = position.x;
	newSprite->transform.translation.y = position.y;
	newSprite->anchorPoint = anchor;
	newSprite->texture = texture;
	newSprite->isUI = true;
	sprites_.push_back(newSprite);
	return *this;
}

// 全部のisActiveを変更する関数
void UI::SetIsActive(bool b) {
	for (int i = 0; i < sprites_.size(); i++) {
		sprites_[i]->isActive = b;
	}
}