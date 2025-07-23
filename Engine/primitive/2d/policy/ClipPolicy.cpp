#include "ClipPolicy.h"
#include "base/ImGuiManager.h"

using namespace LWP::Math;

namespace LWP::Primitive {
	void ClipPolicy::InitImpl(Material* material) {
		clipRect.min = { 0.0f,0.0f };
		clipRect.max = material->texture.t.GetSize();
	}
	void ClipPolicy::UpdateImpl(Vertex* vertices, Material* material) {
		// MinよりMaxのほうが小さくならないように修正
		clipRect.min.x = std::min<float>(clipRect.min.x, clipRect.max.x);
		clipRect.min.y = std::min<float>(clipRect.min.y, clipRect.max.y);
		clipRect.max.x = std::max<float>(clipRect.min.x, clipRect.max.x);
		clipRect.max.y = std::max<float>(clipRect.min.y, clipRect.max.y);

		// 切り抜かれたUV座標を計算
		Vector2 textureSize = material->texture.t.GetSize();	// テクスチャのサイズを取得
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

	void ClipPolicy::DebugGUIImpl() {
		// 切り抜き領域
		ImGui::DragFloat2("clipSize min", &clipRect.min.x, 1.0f);
		ImGui::DragFloat2("clipSize max", &clipRect.max.x, 1.0f);
	}

	void ClipPolicy::PostLoadTextureImpl(Material* material) {
		InitImpl(material);	// 読み込まれたテクスチャに合わせて切り抜き領域を調整
	}

}