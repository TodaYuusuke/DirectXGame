#include "StretchedBillboard.h"

#include "primitive/PrimitiveManager.h"

namespace LWP::Primitive {
	NormalStretchedBillboard::NormalStretchedBillboard() {
		planeType = StretchedBillboard;
		name = "StretchedBillboard";
		anchorPoint = { 0.5f,0.5f };
		Primitive::Manager::GetInstance()->SetBillboardPtr(this);
	};
	NormalStretchedBillboard::~NormalStretchedBillboard() { Primitive::Manager::GetInstance()->DeleteBillboardPtr(this); }

	SequenceStretchedBillboard::SequenceStretchedBillboard() {
		planeType = StretchedBillboard;
		name = "StretchedBillboard";
		anchorPoint = { 0.5f,0.5f };
		Primitive::Manager::GetInstance()->SetBillboardPtr(this);
	};
	SequenceStretchedBillboard::~SequenceStretchedBillboard() { Primitive::Manager::GetInstance()->DeleteBillboardPtr(this); }

	ClipStretchedBillboard::ClipStretchedBillboard() {
		planeType = StretchedBillboard;
		name = "StretchedBillboard";
		anchorPoint = { 0.5f,0.5f };
		Primitive::Manager::GetInstance()->SetBillboardPtr(this);
	};
	ClipStretchedBillboard::~ClipStretchedBillboard() { Primitive::Manager::GetInstance()->DeleteBillboardPtr(this); }
}