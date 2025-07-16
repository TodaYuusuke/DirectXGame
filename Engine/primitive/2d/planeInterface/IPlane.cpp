#include "IPlane.h"
#include "component/Primitive.h"
#include "component/Resource.h"	// テクスチャ読み込み用

using namespace LWP::Math;

namespace LWP::Primitive {
	IPlane::IPlane() { name = ""; }

	void IPlane::Init() {
		IPrimitive::Init();
		anchorPoint = LWP::Math::Vector2{ 0.0f,0.0f };

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

	void IPlane::FitToTexture(Math::Vector2 size) {
		if (planeType != Sprite) {
			size /= 100.0f;	// サイズを調整
			// ビルボード用：XY平面に対してサイズを等倍で展開
			vertices[QuadCorner::TopLeft].position     = { size.x * -anchorPoint.x,         size.y * (1.0f - anchorPoint.y), 0.00f };	// 左上
			vertices[QuadCorner::TopRight].position    = { size.x * (1.0f - anchorPoint.x), size.y * (1.0f - anchorPoint.y), 0.00f };	// 右上
			vertices[QuadCorner::BottomRight].position = { size.x * (1.0f - anchorPoint.x), size.y * -anchorPoint.y,          0.00f };	// 右下
			vertices[QuadCorner::BottomLeft].position  = { size.x * -anchorPoint.x,         size.y * -anchorPoint.y,          0.00f };	// 左下
		}
		else {
			// Sprite（スクリーン座標系）用
			vertices[QuadCorner::TopLeft].position     = { size.x * -anchorPoint.x,         size.y * -anchorPoint.y,         0.00f };	// 左上
			vertices[QuadCorner::TopRight].position    = { size.x * (1.0f - anchorPoint.x), size.y * -anchorPoint.y,         0.00f };	// 右上
			vertices[QuadCorner::BottomRight].position = { size.x * (1.0f - anchorPoint.x), size.y * (1.0f - anchorPoint.y), 0.00f };	// 右下
			vertices[QuadCorner::BottomLeft].position  = { size.x * -anchorPoint.x,         size.y * (1.0f - anchorPoint.y), 0.00f };	// 左下
		}
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