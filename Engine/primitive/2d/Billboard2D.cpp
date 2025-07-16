#include "Billboard2D.h"

#include "primitive/PrimitiveManager.h"

namespace LWP::Primitive {
	NormalBillboard2D::NormalBillboard2D() {
		planeType = Billboard2D;
		name = "NormalBillboard2D";
		anchorPoint = { 0.5f,0.5f };
		Primitive::Manager::GetInstance()->SetBillboard3DPtr(this);
	};

	SequenceBillboard2D::SequenceBillboard2D() {
		planeType = Billboard2D;
		name = "SequenceBillboard2D";
		anchorPoint = { 0.5f,0.5f };
		Primitive::Manager::GetInstance()->SetBillboard3DPtr(this);
	};

	ClipBillboard2D::ClipBillboard2D() {
		planeType = Billboard2D;
		name = "ClipBillboard2D";
		anchorPoint = { 0.5f,0.5f };
		Primitive::Manager::GetInstance()->SetBillboard3DPtr(this);
	};
}