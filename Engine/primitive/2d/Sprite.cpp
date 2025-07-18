#include "Sprite.h"

namespace LWP::Primitive {
	SpriteBuffers::SpriteBuffers()
		: vertices(lwpC::Rendering::Primitive::Sprite::kMaxCount * 4),
		wtf(lwpC::Rendering::Primitive::Sprite::kMaxCount),
		materials(lwpC::Rendering::Primitive::Sprite::kMaxCount) {
		vertices.Init();
		wtf.Init();
		materials.Init();
		count = 0;
	}
	void SpriteBuffers::Reset() {
		vertices.Reset();
		wtf.Reset();
		materials.Reset();
		count = 0;
	}
}