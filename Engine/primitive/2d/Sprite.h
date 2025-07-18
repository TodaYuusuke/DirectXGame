#pragma once
#include "planeInterface/ISprite.h"
#include "base/directX/resource/data/StructuredBuffer.h"
#include "base/directX/renderer/ResourceStruct.h"

namespace LWP::Primitive {
	using NormalSprite = SpriteTemplate<NormalPolicy>;
	using SequenceSprite = SpriteTemplate<SequencePolicy>;
	using ClipSprite = SpriteTemplate<ClipPolicy>;
	
	// スプライトの描画に使うデータ
	struct SpriteBuffers {
		Base::StructuredBuffer<Base::VertexStruct> vertices;		// 頂点
		Base::StructuredBuffer<Base::WTFStruct> wtf;				// ワールドトランスフォーム
		Base::StructuredBuffer<Base::MaterialStruct> materials;		// マテリアル

		// インスタンス数
		int count;

		/// <summary>
		/// デフォルトコンストラクタ
		/// </summary>
		SpriteBuffers();

		/// <summary>
		/// リセットを呼び出す関数
		/// </summary>
		void Reset();
	};
}