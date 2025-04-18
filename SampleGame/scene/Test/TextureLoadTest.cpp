#include "TextureLoadTest.h"

using namespace LWP;

// 初期化
void TextureLoadTest::Initialize() {
	ddsSprite_.material.texture = Resource::LoadTexture("uvChecker.dds");
	pngSprite_.LoadTexture("TestSpriteSheet.png");
}

// 更新
void TextureLoadTest::Update() {
	ImGui::Begin("dds (Left Sprite)");
	ddsSprite_.DebugGUI();
	ImGui::End();
	ImGui::Begin("png (Right Sprite)");
	pngSprite_.DebugGUI();
	ImGui::End();
}