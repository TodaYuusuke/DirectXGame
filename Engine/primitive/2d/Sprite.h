#pragma once
#include "planeInterface/INormal.h"
#include "planeInterface/ISequence.h"
#include "planeInterface/IClip.h"

namespace LWP::Primitive {
	/// <summary>
	/// 通常のビルボード3D
	/// </summary>
	class NormalSprite final : public INormal {
	public:
		NormalSprite();
	};
	/// <summary>
	/// 連番テクスチャ用のビルボード3D
	/// </summary>
	class SequenceSprite final : public ISequence {
	public:
		SequenceSprite();
	};
	/// <summary>
	/// 切り抜き描画用のビルボード3D
	/// </summary>
	class ClipSprite final : public IClip {
	public:
		ClipSprite();
	};
}