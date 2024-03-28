#include "Billboard3D.h"
#include "base/directX/command/CommandManager.h"

using namespace LWP::Primitive;

void Billboard3D::Draw(Base::CommandManager* manager) {
	// アクティブでなければ描画しない
	if (!isActive) { return; }

	manager->SetDrawDataBill3D(this);
}