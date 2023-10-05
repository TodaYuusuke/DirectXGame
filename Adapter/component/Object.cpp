#include "Object.h"

using namespace LWP::System;
using namespace LWP;

template <IsIObject TObject>
static TObject* Object::CreateInstance() {
	return engine_->objectManager_->CreateObjectInstance<TObject>();
}