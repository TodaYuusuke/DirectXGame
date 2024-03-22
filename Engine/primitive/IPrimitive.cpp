#pragma once
#include "IPrimitive.h"
#include "base/directX/command/CommandManager.h"
#include "base/ImGuiManager.h"
#include "component/Primitive.h"

using namespace LWP::Primitive;
using namespace LWP::Resource;
using namespace LWP::Math;

IPrimitive::IPrimitive() {
	// トランスフォーム初期化
	transform.Initialize();

	// 派生クラスでoverrideした関数を基底クラスのコンストラクタで呼び出すことはできないので、
	// 派生クラスに初期化処理を呼び出してもらう

	// 管理クラスにポインタを送信
	SetInstance(this);
}

IPrimitive::~IPrimitive() {
	// 管理クラスのポインタを削除
	DeleteInstance(this);
}

void IPrimitive::Init() {
	// 頂点データ作成
	CreateVertices();
	// インデックスデータ作成
	CreateIndexes();
}

void IPrimitive::CreateVertices() {
	// 頂点のサイズ作成
	vertices.clear();
	vertices.resize(GetVertexCount());
}
void IPrimitive::CreateIndexes() {
	// 頂点のサイズ作成
	indexes.clear();
	indexes.resize(GetIndexCount());

	// 1ループで三角形1つ分のインデックス生成
	for (int i = 0; i < GetVertexCount() - 2; i++) {
		indexes[i * 3] = 0;
		indexes[i * 3 + 1] = i + 1;
		indexes[i * 3 + 2] = i + 2;
	}
}

void IPrimitive::Update() {
	// 特に何もしない
}

void IPrimitive::Draw(Base::CommandManager* manager) {
	// アクティブでなければ描画しない
	if (!isActive) { return; }

	manager->SetDrawData(this);
}

void IPrimitive::DebugGUI(const std::string& label) {
	label;	// 形状に固有名詞が付与されたのでラベルは使用しない（長いこと不要と判断したら消す）
	// 頂点
	if (ImGui::TreeNode("Vertex")) {
		ImGui::SliderInt("vertexNumber", &vertexNum_, 0, GetVertexCount() - 1);
		ImGui::DragFloat3("position", &vertices[vertexNum_].position.x, 0.01f);
		ImGui::DragFloat2("texCoord", &vertices[vertexNum_].texCoord.x, 0.01f);
		ImGui::DragFloat3("normal", &vertices[vertexNum_].normal.x, 0.01f);
		LWP::Base::ImGuiManager::ColorEdit4("color", vertices[vertexNum_].color);
		ImGui::TreePop();
	}
	isUI ? transform.DebugGUI2D() : transform.DebugGUI();
	material.DebugGUI();

	// 共通カラーがあるとき -> ColorEdit4を呼び出す
	if (ImGui::TreeNode("CommonColor")) {
		if (commonColor != nullptr) {
			LWP::Base::ImGuiManager::ColorEdit4("color", *commonColor);
			if (ImGui::Button("Delete CommonColor")) {
				delete commonColor;
				commonColor = nullptr;
			}
		}
		// 共通カラーがないとき -> 共通カラーを作る
		else if (ImGui::Button("Create CommonColor")) {
			commonColor = new Utility::Color(Utility::ColorPattern::WHITE);
		}
		ImGui::TreePop();
	}

	// その他
	// 名前が変更されたとき、ツリーノードを開きっぱなしにするための処理
	if (ImGui::TreeNode("Other")) {
		// 固有名詞変更
		if (ImGui::InputText("Name", &name)) {
			// 名前無しは禁止
			if (name.empty()) { name = "noName"; }
		}
		ImGui::Checkbox("isUI", &isUI);			// 2D描画
		ImGui::Checkbox("isWireFrame", &isWireFrame);	// アクティブ切り替え
		ImGui::Checkbox("isActive", &isActive);	// アクティブ切り替え
		ImGui::Text("- Below this are unique variables - ");
		DerivedDebugGUI();
		ImGui::TreePop();
	}
}

void IPrimitive::DerivedDebugGUI(const std::string& label) {
	// 基底クラスでは記述無し
	label;
}
bool IPrimitive::DerivedGetChanged() {
	// 基底クラスでは何もなし
	return false;
}


int IPrimitive::GetVertexCount() const {
	return 3;
}
int IPrimitive::GetIndexCount() const {
	return (GetVertexCount() - 2) * 3;	// インデックスの数を求める
}

bool IPrimitive::GetChanged() {
	return DerivedGetChanged();
}