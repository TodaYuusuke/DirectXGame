#pragma once
#include "planeInterface/IBillboard.h"

namespace LWP::Primitive {
	using NormalBillboard2D = BillboardTemplate<NormalPolicy, IBillboard2D>;
	using SequenceBillboard2D = BillboardTemplate<SequencePolicy, IBillboard2D>;
	using ClipBillboard2D = BillboardTemplate<ClipPolicy, IBillboard2D>;
}