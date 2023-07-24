#include "Triangle.h"
#include "../../../Adapter/LWP.h"

using namespace LWP::Primitive;

Triangle::Triangle(Manager* controller) {
	primitiveManager = controller;
}

void Triangle::Draw(FillMode fillmode) {
	if (defaultColor != nullptr) {
		for (int i = 0; i < GetVertexCount(); i++) {
			vertices[i].color = *defaultColor;
		}
	}

	primitiveManager->Draw(vertices, GetVertexCount(), fillmode);
}