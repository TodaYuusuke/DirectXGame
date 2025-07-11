#include "Billboard3D.h"

#include "primitive/PrimitiveManager.h"

namespace LWP::Primitive {
	NormalBillboard3D::NormalBillboard3D() {
		planeType = Billboard3D;
		name = "NormalBillboard3D";
		Primitive::Manager::GetInstance()->SetBillboard3DPtr(this);
	};

	SequenceBillboard3D::SequenceBillboard3D() {
		planeType = Billboard3D;
		name = "SequenceBillboard3D";
		Primitive::Manager::GetInstance()->SetBillboard3DPtr(this);
	};

	ClipBillboard3D::ClipBillboard3D() {
		planeType = Billboard3D;
		name = "ClipBillboard3D";
		Primitive::Manager::GetInstance()->SetBillboard3DPtr(this);
	};
}