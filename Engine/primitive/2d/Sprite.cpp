#include "Sprite.h"
#include "../../../Adapter/LWP.h"

using namespace LWP::Primitive;
using namespace LWP::Resource;
using namespace LWP::Math;

Sprite::Sprite(Manager* manager) {
	primitiveManager = manager;
	material = new Material(manager);
	material->data_->enableLighting = false;
	transform.Initialize();
	transform.CreateResource(manager);
}


void Sprite::Draw(FillMode fillmode, Texture* texture) {
	if (defaultColor != nullptr) {
		for (int i = 0; i < GetVertexCount(); i++) {
			vertices[i].color = *defaultColor;
		}
	}

	Vertex3D v[4]{};
	for (int i = 0; i < GetVertexCount(); i++) {
		v[i].position = { vertices[i].position.x,vertices[i].position.y,0.0f };
		v[i].texCoord = vertices[i].texCoord;
		v[i].color = vertices[i].color;
	}
	
	transform.GetMatWorld();	// WorldTransformを更新
	primitiveManager->Draw(v, GetVertexCount(), fillmode, &transform, material, texture, true);
}