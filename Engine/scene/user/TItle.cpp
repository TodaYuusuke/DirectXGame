#include "Title.h"

using namespace LWP::Primitive;
using namespace LWP::Math;
using namespace LWP::Utility;

// 初期化
void Title::Initialize() {
	// 三角形の座標
	for (int i = 0; i < 8; i++) {
		tri[i] = LWP::Engine::CreatePrimitiveInstance<Triangle>();

		tri[i]->vertices[0] = { 0.0f,0.2f,0.0f };
		tri[i]->vertices[1] = { 0.2f,0.0f,0.0f };
		tri[i]->vertices[2] = { -0.2f,0.0f,0.0f };

		tri[i]->transform.translation.y = -0.8f + (i * 0.2f);
	}
	tri[0]->defaultColor = new Color(WHITE);
	tri[1]->defaultColor = new Color(RED);
	tri[2]->defaultColor = new Color(BLUE);
	tri[3]->defaultColor = new Color(GREEN);
	tri[4]->defaultColor = new Color(YELLOW);
	tri[5]->defaultColor = new Color(MAGENTA);
	tri[6]->defaultColor = new Color(CYAN);
	tri[7]->defaultColor = new Color(BLACK);
}
// 更新
void Title::Update() {
	tri[1]->transform.translation.z -= 0.02f;
	tri[2]->transform.translation.z += 0.02f;
	tri[3]->transform.translation.x -= 0.02f;
	tri[4]->transform.translation.x += 0.02f;
}
// 描画
void Title::Draw() {
	for (int i = 0; i < 8; i++) {
		tri[i]->Draw(FillMode::Fill);
	}
}