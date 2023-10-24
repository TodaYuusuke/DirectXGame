#pragma once
#include <Adapter.h>

class Skydome
{
public:

	Skydome();
	~Skydome();
	void Initialze();
	void Update();
	void Draw();

private:

	LWP::Primitive::Mesh* skydome_ = nullptr;

};
