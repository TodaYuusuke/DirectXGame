#include "TextureLoadTest.h"

using namespace LWP;

// 初期化
void TextureLoadTest::Initialize() {
	ddsSprite_.material.texture = Resource::LoadTexture("uvChecker.dds");
	pngSprite_.LoadTexture("TestSpriteSheet.png");
	pngSprite_.SetSplitSize({ 64.0f,64.0f });

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
	ImGuiNE::BeginNode(uniqueId++);
	ImGui::Text("Node A");
	ImGuiNE::BeginPin(uniqueId++, ImGuiNE::PinKind::Input);
	ImGui::Text("-> In");
	ImGuiNE::EndPin();
	ImGui::SameLine();
	ImGuiNE::BeginPin(uniqueId++, ImGuiNE::PinKind::Output);
	ImGui::Text("Out ->");
	ImGuiNE::EndPin();
	ImGuiNE::EndNode();
	ImGuiNE::End();
	ImGuiNE::SetCurrentEditor(nullptr);
}