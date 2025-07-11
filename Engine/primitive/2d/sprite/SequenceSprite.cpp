#include "SequenceSprite.h"

#include "base/ImGuiManager.h"

using namespace LWP::Math;

namespace LWP::Primitive {
	SequenceSprite::SequenceSprite() {
		// 名前を変更（nameには番号がついているのでそれも添付）
		name = "SequenceSprite" + name;
	}

	void SequenceSprite::Init() {
		ISprite::Init();

		index = 0;
		splitSize_ = material.texture.t.GetSize();
	}
	void SequenceSprite::Update() {
		ISprite::Update();

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

	void SequenceSprite::FitToTexture() {
		// 分割サイズで合わせる
		vertices[QuadCorner::TopLeft].position     = { splitSize_.x * -anchorPoint.x,         splitSize_.y * -anchorPoint.y,         0.00f };	// 左上
		vertices[QuadCorner::TopRight].position    = { splitSize_.x * (1.0f - anchorPoint.x), splitSize_.y * -anchorPoint.y,         0.00f };	// 右上
		vertices[QuadCorner::BottomRight].position = { splitSize_.x * (1.0f - anchorPoint.x), splitSize_.y * (1.0f - anchorPoint.y), 0.00f };	// 右下
		vertices[QuadCorner::BottomLeft].position  = { splitSize_.x * -anchorPoint.x,         splitSize_.y * (1.0f - anchorPoint.y), 0.00f };	// 左下
	}
	void SequenceSprite::SetSplitSize(Math::Vector2 size) {
		splitSize_ = size;
		// 座標を合わせておく
		FitToTexture();
	}

	void SequenceSprite::ChildDebugGUI() {
		// インデックス
		ImGui::InputInt("index", &index);
		// 分割サイズ
		ImGui::DragFloat2("splitSize", &splitSize_.x, 1.0f);
	}
}