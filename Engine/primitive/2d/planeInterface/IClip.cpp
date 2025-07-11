#include "IClip.h"
#include "base/ImGuiManager.h"

using namespace LWP::Math;

namespace LWP::Primitive {
	IClip::IClip() {
		name += "Clip";
		Init();
	}

	void IClip::Init() {
		IPlane::Init();

		clipRect.min = { 0.0f,0.0f };
		clipRect.max = material.texture.t.GetSize();
	}
	void IClip::Update() {
		IPlane::Update();

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
	void IClip::LoadTexture(const std::string& fileName) {
		IPlane::LoadTexture(fileName);
		// 読み込み後にサイズ調整
		clipRect.min = { 0.0f,0.0f };
		clipRect.max = material.texture.t.GetSize();
		FitToTexture();
	}


	void IClip::FitToTexture() {
		Vector2 size = clipRect.max - clipRect.min;
		IPlane::FitToTexture(size);
	}
	
	void IClip::ChildDebugGUI() {
		ImGui::DragFloat2("clipSize min", &clipRect.min.x, 1.0f);
		ImGui::DragFloat2("clipSize max", &clipRect.max.x, 1.0f);
	}
}