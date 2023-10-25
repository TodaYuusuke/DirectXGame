#pragma once
#include <Adapter.h>

class Tambourine
{
public:

	Tambourine();
	~Tambourine();
	void Initialize();
	void Update();

	void ShakeBell();

private:

	// モデル
	LWP::Primitive::Mesh* tamModel_ = nullptr;
	LWP::Primitive::Mesh* bellModel_ = nullptr;

	// ベルのシェイク
	LWP::Math::Vector3 shakeBallRotate_{};
	int32_t shakeBallRotateMax_;
	int32_t shakeBallRotateNum_;


};

