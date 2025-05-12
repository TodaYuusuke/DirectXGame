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

	jsonIO_.Init("test.json");
	jsonIO_
		.AddValue("test int32_t", &valueTest[0].i32)
		.AddValue("test float", &valueTest[0].f)
		.AddValue("test vector2", &valueTest[0].v2)
		.AddValue("test vector3", &valueTest[0].v3)
		.AddValue("test string", &valueTest[0].str)
		.AddValue("test transform", &valueTest[0].transform)
		.BeginGroup("Group Test")
			.BeginGroup("in Group Test")
				.AddValue("test int32_t", &valueTest[2].i32)
			.EndGroup()
			.AddValue("test int32_t", &valueTest[1].i32)
			.AddValue<float>("test float", &valueTest[1].f)
			.AddValue("test vector2", &valueTest[1].v2)
			.AddValue("test vector3", &valueTest[1].v3)
			.AddValue("test string", &valueTest[1].str)
		.EndGroup();

	jsonIO_.CheckJsonFile();
;}

// 更新
void TextureLoadTest::Update() {
	ImGui::Begin("dds (Left Sprite)");
	ddsSprite_.DebugGUI();
	ImGui::End();
	ImGui::Begin("png (Right Sprite)");
	pngSprite_.DebugGUI();
	ImGui::End();
	ImGui::Begin("json");
	jsonIO_.DebugGUI();
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