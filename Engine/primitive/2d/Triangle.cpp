#include "Triangle.h"
#include "../../../Adapter/LWP.h"

using namespace LWP::Primitive;
using namespace LWP::Math;

Triangle::Triangle(Manager* controller) {
	primitiveManager = controller;
	transform.Initialize();
}

void Triangle::Draw(FillMode fillmode) {
	if (defaultColor != nullptr) {
		for (int i = 0; i < GetVertexCount(); i++) {
			vertices[i].color = *defaultColor;
		}
	}

	Vertex v[3]{};
	for (int i = 0; i < GetVertexCount(); i++) {
		 v[i].position = vertices[i].position * transform.GetMatWorld();
	}
	
	primitiveManager->Draw(v, GetVertexCount(), fillmode);
}