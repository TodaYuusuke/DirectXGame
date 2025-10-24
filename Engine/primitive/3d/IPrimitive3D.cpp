#include "IPrimitive3D.h"
#include "primitive/PrimitiveManager.h"

namespace LWP::Primitive {
	IPrimitive3D::IPrimitive3D() {
		Manager::GetInstance()->SetPri3DPtr(this);
	}
	IPrimitive3D::~IPrimitive3D() {
		Manager::GetInstance()->DeletePri3DPtr(this);
	}
}