#include "Primitive.h"

using namespace LWP::System;
using namespace LWP;

void Primitive::SetInstance(Primitive::IPrimitive* ptr) {
	engine->primitiveManager_->SetPointer(ptr);
}
void Primitive::DeleteInstance(Primitive::IPrimitive* ptr) {
	engine->primitiveManager_->DeletePointer(ptr);
}

