#include "ISprite.h"
#include "component/Primitive.h"
#include "component/Resource.h"	// テクスチャ読み込み用

using namespace LWP::Math;

namespace LWP::Primitive {
	ISprite::ISprite() {
		Primitive::Manager::GetInstance()->SetSpritePtr(this);	// 管理クラスにポインタをセット
		Init();
		//material.enableLighting = false;	// そもそもスプライトにライテイングは行わないので関係なし
	}
	ISprite::~ISprite() {
		Primitive::Manager::GetInstance()->DeleteSpritePtr(this);	// 管理クラスからポインタを削除
	}

	void ISprite::Init() {
		IPrimitive::Init();
		anchorPoint = LWP::Math::Vector2{ 0.0f,0.0f };

		// 頂点の初期化
		//for (Vertex& v : vertices) { v.Init(); }
		FitToTexture();
		// デフォルトのUV座標
		vertices[QuadCorner::TopLeft].texCoord     = { 0.0f,0.0f };
		vertices[QuadCorner::TopRight].texCoord    = { 1.0f,0.0f };
		vertices[QuadCorner::BottomRight].texCoord = { 1.0f,1.0f };
		vertices[QuadCorner::BottomLeft].texCoord  = { 0.0f,1.0f };
	}
	void ISprite::Update() {
		// アンカーポイントが変わった時用に舞フレーム座標を計算する
		FitToTexture();
	}

	void ISprite::LoadTexture(const std::string& fileName) {
		material.texture = Resource::LoadTexture(fileName);
	}
	void ISprite::FitToTexture() {
		Vector2 size = material.texture.t.GetSize();
		vertices[QuadCorner::TopLeft].position     = { size.x * -anchorPoint.x,         size.y * -anchorPoint.y,         0.00f };	// 左上
		vertices[QuadCorner::TopRight].position    = { size.x * (1.0f - anchorPoint.x), size.y * -anchorPoint.y,         0.00f };	// 右上
		vertices[QuadCorner::BottomRight].position = { size.x * (1.0f - anchorPoint.x), size.y * (1.0f - anchorPoint.y), 0.00f };	// 右下
		vertices[QuadCorner::BottomLeft].position  = { size.x * -anchorPoint.x,         size.y * (1.0f - anchorPoint.y), 0.00f };	// 左下
	}

	void ISprite::DebugGUI() {
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
			CallFitToTextureGUI();	// 初期化呼び出し

			ImGui::TreePop();
		}
		// アクティブ切り替え
		ShowIsActiveGUI();
	}

	void ISprite::ShowAnchorPointGUI() { ImGui::SliderFloat2("anchorPoint", &anchorPoint.x, 0.0f, 1.0f); }
	void ISprite::CallFitToTextureGUI() { if (ImGui::Button("Call FitToTexture")) { FitToTexture(); } }
}