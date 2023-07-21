#include "Title.h"

using namespace LWP::Primitive;

// 初期化
void Title::Initialize() {
	tri[0] = LWP::Engine::CreatePrimitiveInstance<Triangle>();
	tri[1] = LWP::Engine::CreatePrimitiveInstance<Triangle>();

	// 三角形の座標
	const Vector3 ktrianglePos[3] = {
		{ 0.0f,-1.0f,0.0f },
		{ 0.2f,-1.2f,0.0f },
		{ -0.2f,-1.2f,0.0f },
	};
	for (int index = 0; index < 3; index++) {
		translateY[index].y += 0.2f;
	}
	// 三角形の座標
	tri[0]->vertices[0].position = { 0.0f,-1.0f,0.0f };
	tri[0]->vertices[0].color = { 255,255,255,255 };
	tri[0]->vertices[1].position = { 0.2f,-1.2f,0.0f };
	tri[0]->vertices[1].color = { 0,255,0,255 };
	tri[0]->vertices[2].position = { -0.2f,-1.2f,0.0f };
	tri[0]->vertices[2].color = { 255,0,0,255 };

	tri[1]->vertices[0].position = { 0.5f,0.5f,0.0f };
	tri[1]->vertices[0].color = { 255,0,0,255 };
	tri[1]->vertices[1].position = { 0.5f,-0.5f,0.0f };
	tri[1]->vertices[0].color = { 0,0,0,255 };
	tri[1]->vertices[2].position = { 0.2f,0.0f,0.0f };
	tri[1]->vertices[0].color = { 0,0,0,255 };
}
// 更新
void Title::Update() {

}
// 描画
void Title::Draw() {
	// 左側の三角形を描画
	tri[0]->Draw(FillMode::Fill);
	// 右側の三角形を描画
	tri[1]->Draw(FillMode::WireFrame);
}