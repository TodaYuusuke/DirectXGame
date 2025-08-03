#include "IPlane.h"
#include "../policy/IPolicy.h"
#include "component/Resource.h"	// テクスチャ読み込み用

using namespace LWP::Math;

namespace LWP::Primitive {
	void IPlane::Init() {
		IPrimitive::Init();
		anchorPoint = LWP::Math::Vector2{ 0.5f,0.5f };

		// デフォルトのUV座標
		vertices[QuadCorner::TopLeft].texCoord     = { 0.0f,0.0f };
		vertices[QuadCorner::TopRight].texCoord    = { 1.0f,0.0f };
		vertices[QuadCorner::BottomRight].texCoord = { 1.0f,1.0f };
		vertices[QuadCorner::BottomLeft].texCoord  = { 0.0f,1.0f };
	}
	void IPlane::Update() {
		FitToTexture();
	}

	Math::Vector3 IPlane::GetCenterPosition() {
		Math::Vector3 center = { 0.0f,0.0f,0.0f };
		Math::Matrix4x4 mat = worldTF.GetAffineMatrix();
		for (Vertex& v : vertices) {
			center += v.position * mat;
		}
		return center / 4.0f;
	}

	void IPlane::LoadTexture(const std::string& fileName) {
		material.texture = Resource::LoadTexture(fileName);
	}


	void IPlane::FitToTextureSprite(Math::Vector2 size) {
		Vector2 min = { size.x * -anchorPoint.x, size.y * -anchorPoint.y };
		Vector2 max = { size.x * (1.0f - anchorPoint.x), size.y * (1.0f - anchorPoint.y) };

		vertices[QuadCorner::TopLeft].position	   = { min.x, min.y, 0.00f };	// 左上
		vertices[QuadCorner::TopRight].position    = { max.x, min.y, 0.00f };	// 右上
		vertices[QuadCorner::BottomRight].position = { max.x, max.y, 0.00f };	// 右下
		vertices[QuadCorner::BottomLeft].position  = { min.x, max.y, 0.00f };	// 左下
		// Sprite（スクリーン座標系）用
		//vertices[QuadCorner::TopLeft].position     = { size.x * -anchorPoint.x,         size.y * -anchorPoint.y,         0.00f };	// 左上
		//vertices[QuadCorner::TopRight].position    = { size.x * (1.0f - anchorPoint.x), size.y * -anchorPoint.y,         0.00f };	// 右上
		//vertices[QuadCorner::BottomRight].position = { size.x * (1.0f - anchorPoint.x), size.y * (1.0f - anchorPoint.y), 0.00f };	// 右下
		//vertices[QuadCorner::BottomLeft].position  = { size.x * -anchorPoint.x,         size.y * (1.0f - anchorPoint.y), 0.00f };	// 左下
	}
	void IPlane::FitToTextureBillboard(Math::Vector2 size) {
		size /= 100.0f;	// サイズを調整
		Vector2 min = { size.x * -anchorPoint.x, size.y * -anchorPoint.y };
		Vector2 max = { size.x * (1.0f - anchorPoint.x), size.y * (1.0f - anchorPoint.y) };
		vertices[QuadCorner::TopLeft].position     = { min.x, max.y, 0.00f };	// 左上
		vertices[QuadCorner::TopRight].position    = { max.x, max.y, 0.00f };	// 右上
		vertices[QuadCorner::BottomRight].position = { max.x, min.y, 0.00f };	// 右下
		vertices[QuadCorner::BottomLeft].position  = { min.x, min.y, 0.00f };	// 左下
	}

	void IPlane::DebugGUI() {
		// ワールドトランスフォーム
		ShowTransformGUI();
		// マテリアル
		ShowMaterialGUI();
		// アンカーポイント
		ShowAnchorPointGUI();
		// 派生クラス専用の処理
		ChildDebugGUI();
		
		// 詳細な調整用項目
		if (ImGui::TreeNode("--------  Details  --------")) {
			ShowVertexGUI(vertices, 4);	// 頂点
			ShowNameGUI();	// 名前

			ImGui::Text("-----  Functions  -----");
			CallInitGUI();	// 初期化呼び出し
			ShowFitToTextureGUI();	// サイズ調整呼び出し

			ImGui::TreePop();
		}
		// アクティブ切り替え
		ShowIsActiveGUI();
	}

	void IPlane::ShowAnchorPointGUI() { ImGui::SliderFloat2("anchorPoint", &anchorPoint.x, 0.0f, 1.0f); }
	void IPlane::ShowFitToTextureGUI() { if (ImGui::Button("Fit To Texture")) { FitToTexture(); } }

}