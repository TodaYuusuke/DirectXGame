#include "Skydome.h"


Skydome::Skydome(){}

Skydome::~Skydome(){}

void Skydome::Initialze() {

	skydome_ = LWP::Resource::LoadModel("skydome/skydome.obj");
	skydome_->transform.scale = { 16.0f,16.0f,16.0f };
	skydome_->isActive = true;

}

void Skydome::Update() {

}

void Skydome::Draw() {

}
