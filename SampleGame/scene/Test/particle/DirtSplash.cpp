#pragma once
#include "DirtSplash.h"

using namespace LWP::Math;
using namespace LWP::Utility;

DirtSplash::DirtSplash() {
	sprite.material.color = 0x67390aff;
	sprite.worldTF.scale = { 0.05f,0.05f,0.05f };
}

void DirtSplash::Generate(Data& newData) {
	newData.velocity = Random::GenerateVector3({ -0.5f,-1.0f,-1.0f }, { 0.5f,0.0f,1.0f }).Normalize() * Random::GenerateFloat(5.5f, 6.5f);
	newData.velocity.z = 0.0f;
}
bool DirtSplash::UpdateParticle(Data& data) {
	// 重力を加算
	data.velocity.y -= kGravity;
	// 速度ベクトルを加算
	data.s.worldTF.translation += data.velocity;

	// 経過フレーム加算
	data.elapsedTime += LWP::Info::GetDeltaTimeF();
	// kDeleteTime秒かけて消滅アニメーションを完了したとき消滅する
	if (data.elapsedTime >= kDeleteTime) {
		return true;
	}
	return false;
}