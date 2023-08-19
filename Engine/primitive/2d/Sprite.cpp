#include "Sprite.h"
#include "../../../Adapter/LWP.h"

using namespace LWP::Primitive;
using namespace LWP::Resource;
using namespace LWP::Math;

Sprite::Sprite(Manager* manager) {
	primitiveManager = manager;
	transform.Initialize();
}

void Sprite::Draw(FillMode fillmode, Texture* texture) {
	if (defaultColor != nullptr) {
		for (int i = 0; i < GetVertexCount(); i++) {
			vertices[i].color = *defaultColor;
		}
	}

	Vertex3D v[4]{};
	for (int i = 0; i < GetVertexCount(); i++) {
		v[i].position = vertices[i].position * transform.GetMatWorld();
		v[i].texCoord = vertices[i].texCoord;
		v[i].color = vertices[i].color;
	}

	primitiveManager->Draw(v, GetVertexCount(), fillmode, texture, true);
}