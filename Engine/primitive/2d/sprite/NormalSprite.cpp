#include "NormalSprite.h"


namespace LWP::Primitive {
	NormalSprite::NormalSprite() {
		// 名前を変更（nameには番号がついているのでそれも添付）
		name = "NormalSprite" + name;
	}
}