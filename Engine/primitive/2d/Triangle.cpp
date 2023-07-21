#include "Triangle.h"
#include "../../../Adapter/LWP.h"

using namespace LWP::Primitive;

Triangle::Triangle(Manager* controller) {
	controller_ = controller;
}

void Triangle::Draw(FillMode fillmode) {
	controller_->Draw(vertices, GetVertexCount(), fillmode);
}