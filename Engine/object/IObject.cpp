#include "IObject.h"
#include "base/directX/RendererManager.h"

#include "component/Object.h"

using namespace LWP::Object;

IObject::IObject() { 
	LWP::Information::OutputLog::Output("Create IObject\n");
	SetPointer(this); }
IObject::~IObject() { DeletePointer(this); }
