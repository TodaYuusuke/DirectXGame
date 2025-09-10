#pragma once
#include "TracesofDirt.h"

using namespace LWP::Math;
using namespace LWP::Utility;

TracesofDirt::TracesofDirt() {
	sprite.LoadTexture("TracesofDirt.png");
	sprite.material.color.A = 80;
	sprite.SetSplitSize({ 128.0f,128.0f });
	sprite.worldTF.scale = { 0.5f,0.5f,1.0f };
}

void TracesofDirt::Generate(Data& newData) {
	newData.s.index = Random::GenerateInt(0, 4);
	Vector2 pos = Random::GenerateVector2(kGenerateMinPoint, kGenerateMaxPoint);
	newData.s.worldTF.translation.x = pos.x;
	newData.s.worldTF.translation.y = pos.y;
	newData.s.worldTF.translation += Random::GenerateVector3({ -1.0f,-1.0f,-1.0f }, { 1.0f,1.0f,1.0f }).Normalize() * kGenerateRange;
}
bool TracesofDirt::UpdateParticle(Data& data) {
	// 経過フレーム加算
	data.elapsedTime += LWP::Info::GetDeltaTimeF();
	// kDeleteTime秒かけて消滅アニメーションを完了したとき消滅する
	if (data.elapsedTime >= kDeleteTime) {
		return true;
	}
	return false;
}