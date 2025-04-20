#pragma once
#include <Adapter.h>
#include "scene/IScene.h"

class TextureLoadTest final
	: public IScene {
public:
	//*** 純粋仮想関数の実体宣言 ***//

	// 初期化
	void Initialize() override;
	// 更新
	void Update() override;


private: //*** これより先に必要な処理や変数を記述 ***//

	// ddsのスプライト
	LWP::Primitive::Sprite ddsSprite_;
	// pngのスプライト
	LWP::Primitive::Sprite pngSprite_;
	LWP::Primitive::Billboard2D bill2D;
	LWP::Primitive::Billboard3D bill3D;

	ImGui::NE::EditorContext* m_Context = nullptr;
};