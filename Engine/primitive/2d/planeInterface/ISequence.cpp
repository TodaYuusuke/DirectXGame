#include "ISequence.h"

#include "base/ImGuiManager.h"

using namespace LWP::Math;

namespace LWP::Primitive {
	ISequence::ISequence() {
		name += "Sequence";
		Init();
	}

	void ISequence::Init() {
		IPlane::Init();

		index = 0;
		splitSize_ = material.texture.t.GetSize();
	}
	void ISequence::Update() {
		IPlane::Update();

		// 分割されたUV座標を計算
		Vector2 textureSize = material.texture.t.GetSize();	// テクスチャのサイズを取得
		float splitUVx = splitSize_.x / textureSize.x;
		float splitUVy = splitSize_.y / textureSize.y;

		// 分割画像上のXY座標を求める
		float x = (index * splitUVx) - static_cast<int>(index * splitUVx);
		float y = static_cast<int>(index * splitUVx) * splitUVy;

		// デフォルトのテクスチャ座標
		vertices[QuadCorner::TopLeft].texCoord     = { x,            y };
		vertices[QuadCorner::TopRight].texCoord    = { x + splitUVx, y };
		vertices[QuadCorner::BottomRight].texCoord = { x + splitUVx, y + splitUVy };
		vertices[QuadCorner::BottomLeft].texCoord  = { x,            y + splitUVy };
	}

	void ISequence::FitToTexture() {
		IPlane::FitToTexture(splitSize_);
	}
	void ISequence::SetSplitSize(Math::Vector2 size) {
		splitSize_ = size;
	}

	void ISequence::ChildDebugGUI() {
		// インデックス
		ImGui::InputInt("index", &index);
		// 分割サイズ
		ImGui::DragFloat2("splitSize", &splitSize_.x, 1.0f);
	}
}