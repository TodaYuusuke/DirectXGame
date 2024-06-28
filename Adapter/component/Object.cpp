#include "Object.h"

using namespace LWP::System;
using namespace LWP;

void Object::SetPointer(Object::IObject* ptr) {
	engine->objectManager_->SetPointer(ptr);
}
void Object::DeletePointer(Object::IObject* ptr) {
	engine->objectManager_->DeletePointer(ptr);
}