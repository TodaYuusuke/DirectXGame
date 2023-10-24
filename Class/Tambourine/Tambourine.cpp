#include "Tambourine.h"

Tambourine::Tambourine() {

}

Tambourine::~Tambourine() {

}

void Tambourine::Initialize() {

	model_ = LWP::Resource::LoadModel("tan/tan.obj");
	model_->isActive = true;
	model_->transform.scale = { 2.2f,2.4f,2.2f };

}
void Tambourine::Update() {

}
