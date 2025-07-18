#pragma once
#include "planeInterface/INormal.h"
#include "planeInterface/ISequence.h"
#include "planeInterface/IClip.h"

namespace LWP::Primitive {
	/// <summary>
	/// 通常のストレッチビルボード
	/// </summary>
	class NormalStretchedBillboard final : public INormal {
	public:
		// 速度ベクトル（自動でworldTFに加算されないので注意）
		Math::Vector3 velocity;

		NormalStretchedBillboard();
		~NormalStretchedBillboard();
	};
	/// <summary>
	/// 連番テクスチャ用のストレッチビルボード
	/// </summary>
	class SequenceStretchedBillboard final : public ISequence {
	public:
		// 速度ベクトル（自動でworldTFに加算されないので注意）
		Math::Vector3 velocity;

		SequenceStretchedBillboard();
		~SequenceStretchedBillboard();
	};
	/// <summary>
	/// 切り抜き描画用のストレッチビルボード
	/// </summary>
	class ClipStretchedBillboard final : public IClip {
	public:
		// 速度ベクトル（自動でworldTFに加算されないので注意）
		Math::Vector3 velocity;
		
		ClipStretchedBillboard();
		~ClipStretchedBillboard();
	};
}