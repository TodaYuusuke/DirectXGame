#include "PrimitiveManager.h"

using namespace LWP::Primitive;

void Manager::Initialize() {
	for (IPrimitive* p : primitives_) {
		delete p;
	}
	primitives_.clear();
}

void Manager::Draw(Base::CommandManager* manager) {
	// 描画
	for (IPrimitive* p : primitives_) {
		p->Draw(manager);
	}
}