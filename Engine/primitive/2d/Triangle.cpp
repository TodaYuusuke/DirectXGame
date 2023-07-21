#include "Triangle.h"
#include "../../../Adapter/LWP.h"

using namespace LWP::Primitive;

Triangle::Triangle(Manager* controller) {
	primitiveManager = controller;
}

void Triangle::Draw(FillMode fillmode) {
	primitiveManager->Draw(vertices, GetVertexCount(), fillmode);
}