#include "TextureLoadTest.h"

using namespace LWP;

// 初期化
void TextureLoadTest::Initialize() {
	//pngSprite_.material.texture = Resource::LoadTexture("uvChecker.png");
	//ddsSprite_.material.texture = Resource::LoadTexture("uvChecker.dds");
	//ddsSprite_.worldTF.translation.x = 700.0f;
}

// 更新
void TextureLoadTest::Update() {
	ImGui::Begin("png (Left Sprite)");
	pngSprite_.DebugGUI();
	ImGui::End();
	ImGui::Begin("dds (Right Sprite)");
	ddsSprite_.DebugGUI();
	ImGui::End();
}