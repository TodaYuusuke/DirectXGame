#include "Surface.h"
#include "../../../Adapter/LWP.h"

using namespace LWP::Primitive;
using namespace LWP::Resource;
using namespace LWP::Math;

Surface::Surface(Manager* manager) {
	primitiveManager = manager;
	material = new Material(manager);
	material->data_->enableLighting = true;
	transform.Initialize();
	transform.CreateResource(manager);
}


void Surface::Draw(FillMode fillmode, Texture* texture) {
	if (defaultColor != nullptr) {
		for (int i = 0; i < GetVertexCount(); i++) {
			vertices[i].color = *defaultColor;
		}
	}

	transform.GetMatWorld();	// WorldTransformを更新
	primitiveManager->Draw(vertices, GetVertexCount(), fillmode, &transform, material, texture, false);
}