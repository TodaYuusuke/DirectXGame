#include "Sprite.h"

#include "primitive/PrimitiveManager.h"

namespace LWP::Primitive {
	NormalSprite::NormalSprite() {
		planeType = Sprite;
		name = "NormalSprite";
		Primitive::Manager::GetInstance()->SetSpritePtr(this);
	};
	NormalSprite::~NormalSprite() { Primitive::Manager::GetInstance()->DeleteSpritePtr(this); }
	SequenceSprite::SequenceSprite() {
		planeType = Sprite;
		name = "SequenceSprite";
		Primitive::Manager::GetInstance()->SetSpritePtr(this);
	};
	SequenceSprite::~SequenceSprite() { Primitive::Manager::GetInstance()->DeleteSpritePtr(this); }
	ClipSprite::ClipSprite() {
		planeType = Sprite;
		name = "ClipSprite";
		Primitive::Manager::GetInstance()->SetSpritePtr(this);
	};
	ClipSprite::~ClipSprite() { Primitive::Manager::GetInstance()->DeleteSpritePtr(this); }
}