#include "TextureLoadTest.h"

using namespace LWP;

// 初期化
void TextureLoadTest::Initialize() {
	ddsSprite_.name = "DDS";
	ddsSprite_.material.texture = Resource::LoadTexture("uvChecker.dds");
	pngSprite_.name = "PNG";
	pngSprite_.LoadTexture("TestSpriteSheet.png");
	pngSprite_.SetSplitSize({ 64.0f,64.0f });
	bill2D.name = "Bill2D";
	bill2D.LoadTexture("TestSpriteSheet.png");
	bill2D.SetSplitSize({ 64.0f,64.0f });
	bill2D.anchorPoint = { 0.5f,0.5f };
	bill3D.name = "Bill3D";
	bill3D.LoadTexture("TestSpriteSheet.png");
	bill3D.SetSplitSize({ 64.0f,64.0f });

	ImGui::NE::Config config;
	config.SettingsFile = "resources/json/Simple.json";
	m_Context = ImGui::NE::CreateEditor(&config);
}

// 更新
void TextureLoadTest::Update() {
	ImGui::Begin("dds (Left Sprite)");
	ddsSprite_.DebugGUI();
	ImGui::End();
	ImGui::Begin("png (Right Sprite)");
	pngSprite_.DebugGUI();
	ImGui::End();

	ImGuiNE::SetCurrentEditor(m_Context);
	ImGuiNE::Begin("My Editor", ImVec2(0.0, 0.0f));
	int uniqueId = 1;
	// Start drawing nodes.
	ImGuiNE::BeginNode(1);
	ImGui::Text("Node A");
	ImGuiNE::BeginPin(2, ImGuiNE::PinKind::Input);
	ImGui::Text("-> In");
	ImGuiNE::EndPin();
	ImGui::SameLine();
	ImGuiNE::BeginPin(3, ImGuiNE::PinKind::Output);
	ImGui::Text("Out ->");
	ImGuiNE::EndPin();
	ImGuiNE::EndNode();

	ImGuiNE::BeginNode(4);
	ImGui::Text("Node B");
	ImGuiNE::BeginPin(5, ImGuiNE::PinKind::Input);
	ImGui::Text("-> In");
	ImGuiNE::EndPin();
	ImGui::SameLine();
	ImGuiNE::BeginPin(6, ImGuiNE::PinKind::Output);
	ImGui::Text("Out ->");
	ImGuiNE::EndPin();
	ImGuiNE::EndNode();

	ImGuiNE::End();
	ImGuiNE::SetCurrentEditor(nullptr);
}