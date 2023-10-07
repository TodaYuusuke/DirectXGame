#include "ObjectManager.h"

using namespace LWP::Object;

void Manager::Initialize() {
	for (IObject* o : objects_) {
		delete o;
	}
}

void Manager::Update() {
	for (IObject* o : objects_) {
		o->Update();
	}
}

void Manager::Draw() {
	for (IObject* o : objects_) {
		o->Draw();
	}
}