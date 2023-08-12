#include "Triangle.h"
#include "../../../Adapter/LWP.h"

using namespace LWP::Primitive;
using namespace LWP::Resource;
using namespace LWP::Math;

Triangle::Triangle(Manager* manager) {
	primitiveManager = manager;
	transform.Initialize();
}

void Triangle::Draw(FillMode fillmode, Texture* texture) {
	if (defaultColor != nullptr) {
		for (int i = 0; i < GetVertexCount(); i++) {
			vertices[i].color = *defaultColor;
		}
	}

	Vertex v[3]{};
	for (int i = 0; i < GetVertexCount(); i++) {
		 v[i].position = vertices[i].position * transform.GetMatWorld();
		 v[i].texCoord = vertices[i].texCoord;
		 v[i].color = vertices[i].color;
	}
	
	primitiveManager->Draw(v, GetVertexCount(), fillmode, texture);
}