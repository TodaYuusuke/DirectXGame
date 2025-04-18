#include "TextureLoadTest.h"

using namespace LWP;

// 初期化
void TextureLoadTest::Initialize() {
	pngSprite_.texture = Resource::LoadTexture("uvChecker.png");
	ddsSprite_.texture = Resource::LoadTexture("uvChecker.dds");
}

// 更新
void TextureLoadTest::Update() {
	pngSprite_.Draw({ 0.0f,0.0f });
	ddsSprite_.Draw({ 0.0f,200.0f });

	ImGui::Begin("png (Left Sprite)");
	pngSprite_.DebugGUI();
	ImGui::End();
	ImGui::Begin("dds (Right Sprite)");
	ddsSprite_.DebugGUI();
	ImGui::End();
}