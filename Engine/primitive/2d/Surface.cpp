#include "Surface.h"
#include "../../../Adapter/LWP.h"

using namespace LWP::Primitive;
using namespace LWP::Resource;
using namespace LWP::Math;

Surface::Surface(Manager* manager) {
	primitiveManager = manager;
	material = new Material(manager);
	material->data_->enableLighting = false;
	transform.Initialize();
	transform.CreateResource(manager);
}


void Surface::Draw(FillMode fillmode, Texture* texture) {
	if (defaultColor != nullptr) {
		for (int i = 0; i < GetVertexCount(); i++) {
			vertices[i].color = *defaultColor;
		}
	}

	transform.MatWorld();	// WorldTransformを更新
	primitiveManager->Draw(vertices, GetVertexCount(), nullptr, 0, fillmode, &transform, material, texture, isUI);
}