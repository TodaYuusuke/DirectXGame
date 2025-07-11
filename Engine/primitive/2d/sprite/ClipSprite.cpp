#include "ClipSprite.h"
#include "base/ImGuiManager.h"

using namespace LWP::Math;

namespace LWP::Primitive {
	ClipSprite::ClipSprite() {
		// 名前を変更（nameには番号がついているのでそれも添付）
		name = "ClipSprite" + name;
	}

	void ClipSprite::Init() {
		ISprite::Init();

		clipRect.min = { 0.0f,0.0f };
		clipRect.max = material.texture.t.GetSize();
	}
	void ClipSprite::Update() {
		ISprite::Update();

		// MinよりMaxのほうが小さくならないように修正
		clipRect.min.x = std::min<float>(clipRect.min.x, clipRect.max.x);
		clipRect.min.y = std::min<float>(clipRect.min.y, clipRect.max.y);
		clipRect.max.x = std::max<float>(clipRect.min.x, clipRect.max.x);
		clipRect.max.y = std::max<float>(clipRect.min.y, clipRect.max.y);

		// 切り抜かれたUV座標を計算
		Vector2 textureSize = material.texture.t.GetSize();	// テクスチャのサイズを取得
		Rectangle2D uv = {
			{
				clipRect.min.x / textureSize.x,
				clipRect.min.y / textureSize.y
			},{
				clipRect.max.x / textureSize.x,
				clipRect.max.y / textureSize.y
			},
		};

		// デフォルトのテクスチャ座標
		vertices[QuadCorner::TopLeft].texCoord     = { uv.min.x, uv.min.y };
		vertices[QuadCorner::TopRight].texCoord    = { uv.max.x, uv.min.y };
		vertices[QuadCorner::BottomRight].texCoord = { uv.max.x, uv.max.y };
		vertices[QuadCorner::BottomLeft].texCoord  = { uv.min.x, uv.max.y };
	}
	void ClipSprite::LoadTexture(const std::string& fileName) {
		ISprite::LoadTexture(fileName);
		// 読み込み後にサイズ調整
		clipRect.min = { 0.0f,0.0f };
		clipRect.max = material.texture.t.GetSize();
		FitToTexture();
	}


	void ClipSprite::FitToTexture() {
		Vector2 size = clipRect.max - clipRect.min;

		// 切り抜く領域に合わせる
		vertices[QuadCorner::TopLeft].position     = { size.x * -anchorPoint.x,         size.y * -anchorPoint.y,         0.00f };	// 左上
		vertices[QuadCorner::TopRight].position    = { size.x * (1.0f - anchorPoint.x), size.y * -anchorPoint.y,         0.00f };	// 右上
		vertices[QuadCorner::BottomRight].position = { size.x * (1.0f - anchorPoint.x), size.y * (1.0f - anchorPoint.y), 0.00f };	// 右下
		vertices[QuadCorner::BottomLeft].position  = { size.x * -anchorPoint.x,         size.y * (1.0f - anchorPoint.y), 0.00f };	// 左下
	}
	
	void ClipSprite::ChildDebugGUI() {
		ImGui::DragFloat2("clipSize min", &clipRect.min.x, 1.0f);
		ImGui::DragFloat2("clipSize max", &clipRect.max.x, 1.0f);
	}
}