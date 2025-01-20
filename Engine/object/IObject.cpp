#include "IObject.h"
#include "base/directX/RendererManager.h"

#include "component/Object.h"

using namespace LWP::Object;

IObject::IObject() { Object::Manager::GetInstance()->SetPointer(this); }
IObject::~IObject() { Object::Manager::GetInstance()->DeletePointer(this); }
