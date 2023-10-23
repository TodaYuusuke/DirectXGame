#pragma once
#include <Adapter.h>

class Tambourine
{
public:

	Tambourine();
	~Tambourine();
	void Initialize();
	void Update();


private:

	LWP::Primitive::Mesh* model_;

};

