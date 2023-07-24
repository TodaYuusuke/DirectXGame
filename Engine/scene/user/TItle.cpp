#include "Title.h"

using namespace LWP::Primitive;
using namespace LWP::Math;
using namespace LWP::Utility;

// 初期化
void Title::Initialize() {
	// 三角形の座標
	for (int i = 0; i < 10; i++) {
		tri[i] = LWP::Engine::CreatePrimitiveInstance<Triangle>();

		tri[i]->vertices[0] = { 0.0f,-0.8f + (i * 0.2f),0.0f };
		tri[i]->vertices[1] = { 0.2f,-1.0f + (i * 0.2f),0.0f };
		tri[i]->vertices[2] = { -0.2f,-1.0f + (i * 0.2f),0.0f };
		
		tri[i]->defaultColor = new Color(RED);
	}
}
// 更新
void Title::Update() {

}
// 描画
void Title::Draw() {
	for (int i = 0; i < 10; i++) {
		tri[i]->Draw(FillMode::Fill);
	}
}