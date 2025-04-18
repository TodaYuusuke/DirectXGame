#include "TextureLoadTest.h"

using namespace LWP;

// 初期化
void TextureLoadTest::Initialize() {
	//pngSprite_.material.texture = Resource::LoadTexture("uvChecker.png");
	//ddsSprite_.material.texture = Resource::LoadTexture("uvChecker.dds");
	//ddsSprite_.worldTF.translation.x = 700.0f;

    ImGui::NE::Config config;
    config.SettingsFile = "Simple.json";
    m_Context = ImGui::NE::CreateEditor(&config);
}

// 更新
void TextureLoadTest::Update() {
	ImGui::Begin("png (Left Sprite)");
	pngSprite_.DebugGUI();
	ImGui::End();
	ImGui::Begin("dds (Right Sprite)");
	ddsSprite_.DebugGUI();
	ImGui::End();

	ImGui::Begin("Test");

    ImGui::NE::SetCurrentEditor(m_Context);
    ImGui::NE::Begin("My Editor", ImVec2(0.0, 0.0f));
    int uniqueId = 1;
    static float f = 0.0f;
    // Start drawing nodes.
    ImGui::NE::BeginNode(uniqueId++);
    ImGui::Text("Node A");
    ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
    ImGui::NE::BeginPin(uniqueId++, ImGui::NE::PinKind::Input);
    ImGui::Text("-> In");
    ImGui::NE::EndPin();
    ImGui::SameLine();
    ImGui::NE::BeginPin(uniqueId++, ImGui::NE::PinKind::Output);
    ImGui::Text("Out ->");
    ImGui::NE::EndPin();
    ImGui::NE::EndNode();
    ImGui::NE::End();
    ImGui::NE::SetCurrentEditor(nullptr);
	

	ImGui::End();
}