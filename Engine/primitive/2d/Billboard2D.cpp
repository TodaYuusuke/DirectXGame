#include "Billboard2D.h"
#include "base/directX/command/CommandManager.h"

using namespace LWP::Primitive;

void Billboard2D::Draw(Base::CommandManager* manager) {
	// アクティブでなければ描画しない
	if (!isActive) { return; }

	manager->SetDrawDataBill2D(this);
}
