#include "Sprite.h"

#include "primitive/PrimitiveManager.h"

namespace LWP::Primitive {
	NormalSprite::NormalSprite() {
		planeType = Sprite;
		name = "NormalSprite";
		Primitive::Manager::GetInstance()->SetSpritePtr(this);
	};
	SequenceSprite::SequenceSprite() {
		planeType = Sprite;
		name = "SequenceSprite";
		Primitive::Manager::GetInstance()->SetSpritePtr(this);
	};
	ClipSprite::ClipSprite() {
		planeType = Sprite;
		name = "ClipSprite";
		Primitive::Manager::GetInstance()->SetSpritePtr(this);
	};
}