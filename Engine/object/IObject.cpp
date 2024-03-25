#include "IObject.h"

#include "component/Object.h"

using namespace LWP::Object;

IObject::IObject() { SetPointer(this); }
IObject::~IObject() { DeletePointer(this); }
