#pragma once
#include "planeInterface/INormal.h"
#include "planeInterface/ISequence.h"
#include "planeInterface/IClip.h"

namespace LWP::Primitive {
	/// <summary>
	/// 通常のビルボード3D
	/// </summary>
	class NormalBillboard2D final : public INormal {
	public:
		NormalBillboard2D();
		~NormalBillboard2D();
	};
	/// <summary>
	/// 連番テクスチャ用のビルボード3D
	/// </summary>
	class SequenceBillboard2D final : public ISequence {
	public:
		SequenceBillboard2D();
		~SequenceBillboard2D();
	};
	/// <summary>
	/// 切り抜き描画用のビルボード3D
	/// </summary>
	class ClipBillboard2D final : public IClip {
	public:
		ClipBillboard2D();
		~ClipBillboard2D();
	};
}