#include "ObjectManager.h"
#include "IObject.h"

using namespace LWP::Object;

void Manager::Initialize() {
	for (IObject* o : objects) {
		delete o;
	}
}

void Manager::Update() {
	for (IObject* o : objects) {
		o->Update();
	}
}

void Manager::Draw() {
	for (IObject* o : objects) {
		o->Draw();
	}
}